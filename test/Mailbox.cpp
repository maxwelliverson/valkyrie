//
// Created by Maxwell on 2020-12-13.
//

#include <vector>
#include <thread>
#include <future>
#include <atomic>
#include <sstream>
#include <iostream>
#include <cassert>
#include <barrier>
#include <span>
#include <iomanip>
#include <fstream>
#include <ctime>

#define NOMINMAX

#include <windows.h>
#include <memoryapi.h>
#include <errhandlingapi.h>

inline static uint32_t WriteThreadId = 0;
inline static uint32_t ReadThreadId = 0;

enum class MessageState : uint32_t {
  Invalid,
  Written,
  Read,
  Processed,
  Final
};
struct MessageProperties{
  uint32_t     nextOffset = 0;
  MessageState state      = MessageState::Invalid;
};

using AtomicMessageProperties = std::atomic<MessageProperties>;

inline constexpr static uint32_t NotYetWritten = static_cast<uint32_t>(-1);
inline constexpr static uint32_t AcquireUnique = static_cast<uint32_t>(-1);
inline constexpr static uint32_t AlreadyRead   = static_cast<uint32_t>(-2);
inline constexpr static uint32_t LastMessageIndexPlaceholder = static_cast<uint32_t>(-3);

struct ReadThread{
  std::thread threadObject{};
  uint32_t    id = ReadThreadId++;
  uint32_t    readCount = 0;
  std::stringstream outBuffer{};
};
struct WriteThread{
  std::thread          threadObject{};
  std::string          id = std::to_string(WriteThreadId++);
  std::atomic_uint32_t writeCount = 0;
};

struct Message{
  std::atomic<MessageState> state = MessageState::Invalid;
  std::atomic_uint32_t      nextOffset  = uint32_t(-1);
  uint32_t                  msgLength   = 0;
  char                      pData[];
};

uint32_t nextOffset(std::string_view msgString) noexcept {
  const uint32_t unalignedSize = msgString.length() + offsetof(Message, pData);
  const uint32_t alignmentMask = alignof(Message) - 1;
  const uint32_t additionalAlignment = bool(unalignedSize & alignmentMask) * ((~unalignedSize & alignmentMask) + 1);
  return unalignedSize + additionalAlignment;
  //return unalignedSize + ~(bool(unalignedSize & alignmentMask) - 1U)
}




struct Buffer{
  void*                pBuffer;
  uint32_t             bufferSize;
  std::atomic_uint32_t readCompleteOffset;

  std::atomic_uint32_t nextReadOffset;
  std::atomic_uint32_t nextWriteOffset;
};



template <typename T, typename Alloc = std::allocator<T>>
class TempSpan : public Alloc, public std::span<T>{
public:
  explicit TempSpan(size_t count)
      : Alloc(),
        std::span<T>(std::allocator_traits<Alloc>::allocate(*this, count), count){
    for (auto& val : *this)
      new(std::addressof(val)) T;
  }
  TempSpan(size_t count, const Alloc& allocator)
      : Alloc(allocator),
        std::span<T>(std::allocator_traits<Alloc>::allocate(*this, count), count){
    for (auto& val : *this)
      new(std::addressof(val)) T;
  }
  TempSpan(const TempSpan&) = delete;
  TempSpan(TempSpan&& other) noexcept : Alloc(std::move(other)), std::span<T>(other){
    static_cast<std::span<T>&>(other) = {};
  }
  ~TempSpan(){
    if (this->data()) {
      if constexpr (!std::is_trivially_destructible_v<T>) {
        for (auto&& obj : *this)
          obj.~T();
      }
      std::allocator_traits<Alloc>::deallocate(*this, this->data(), this->size());
    }
  }
};


using std::chrono::duration_cast;

using Clock    = std::chrono::high_resolution_clock;
using Duration = Clock::duration;
using DoubleDuration = std::chrono::duration<double, Duration::period>;
using TimePoint = Clock::time_point;

struct BasicTimer{
  TimePoint startTime;
  TimePoint endTime;
};

struct ThreadTimer : BasicTimer{
  std::vector<BasicTimer> calls;
};

struct AsyncTimer : BasicTimer{
  std::vector<ThreadTimer> threads;
};
struct IterationTimer : BasicTimer{
  AsyncTimer read;
  AsyncTimer write;
};
struct ProcessTimer : BasicTimer{
  std::vector<IterationTimer> iterations;
};

struct ScopedTimer{
  BasicTimer& timer;
  ScopedTimer(BasicTimer& timer) noexcept
      : timer(timer){
    timer.startTime = Clock::now();
  }
  ~ScopedTimer(){
    timer.endTime = Clock::now();
  }
};


struct BasicTimerStats{
  DoubleDuration ave = DoubleDuration::zero();
  Duration       max = Duration::min();
  Duration       min = Duration::max();
};
struct ExtendedTimerStats : BasicTimerStats{
  DoubleDuration maxAve = DoubleDuration::min();
  DoubleDuration minAve = DoubleDuration::max();
  DoubleDuration aveMax = DoubleDuration::zero();
  Duration       minMax = Duration::max();
  DoubleDuration aveMin = DoubleDuration::zero();
  Duration       maxMin = Duration::min();
};


struct ThreadTimerResult{
  Duration totalTime = Duration::zero();
  BasicTimerStats callTimes{};
  std::vector<Duration> call{};
};
struct AsyncTimerResult{
  Duration     totalTime = Duration::zero();
  BasicTimerStats   threadTimes{};
  ExtendedTimerStats callTimes{};
  std::vector<ThreadTimerResult> threads{};
};
struct IterationTimerResult{
  Duration totalTime = Duration::zero();
  AsyncTimerResult read{};
  AsyncTimerResult write{};
};


struct ProcessIterationResult{
  BasicTimerStats    asyncTimes;
  ExtendedTimerStats threadTimes;
  ExtendedTimerStats callTimes;
};



void addStat(BasicTimerStats& stats,       Duration time){
  stats.ave += time;
  if (stats.max < time)
    stats.max = time;
  if (stats.min > time)
    stats.min = time;
}
void addStat(ExtendedTimerStats& extStats, const BasicTimerStats& basicStats){
  extStats.ave    += basicStats.ave;
  extStats.aveMax += basicStats.max;
  extStats.aveMin += basicStats.min;


  if (extStats.max < basicStats.max)
    extStats.max = basicStats.max;
  if (extStats.min > basicStats.min)
    extStats.min = basicStats.min;

  if (extStats.maxMin < basicStats.min)
    extStats.maxMin = basicStats.min;
  if (extStats.minMax > basicStats.max)
    extStats.minMax = basicStats.max;


  if (extStats.maxAve < basicStats.ave)
    extStats.maxAve = basicStats.ave;
  if (extStats.minAve > basicStats.ave)
    extStats.minAve = basicStats.ave;
}

void normalize(BasicTimerStats& stats,    size_t count){
  stats.ave /= count;
}
void normalize(ExtendedTimerStats& stats, size_t count){
  stats.ave    /= count;
  stats.aveMax /= count;
  stats.aveMin /= count;
}


struct ProcessTimerResult{
  Duration               totalTime;
  BasicTimerStats        iterationTimes;
  ProcessIterationResult read;
  ProcessIterationResult write;
  std::vector<IterationTimerResult> iterations;

  ProcessTimerResult(const ProcessTimer& timer) noexcept
      : totalTime(timer.endTime - timer.startTime),
        iterations(timer.iterations.size()){
    const size_t iterationCount = timer.iterations.size();

    for (auto i = 0; i < iterationCount; ++i){
      const auto& iterationTimer = timer.iterations[i];
      auto& iteration = iterations[i];


      iteration.totalTime = iterationTimer.endTime - iterationTimer.startTime;
      addStat(iterationTimes, iteration.totalTime);

      const size_t readSize = iterationTimer.read.threads.size();
      const size_t writeSize = iterationTimer.write.threads.size();

      iteration.read.threads.resize(readSize);
      iteration.write.threads.resize(writeSize);

      iteration.read.totalTime =  iterationTimer.read.endTime - iterationTimer.read.startTime;
      iteration.write.totalTime = iterationTimer.write.endTime - iterationTimer.write.startTime;

      addStat(read.asyncTimes,  iteration.read.totalTime);
      addStat(write.asyncTimes, iteration.write.totalTime);



      for (auto r = 0; r < readSize; ++r) {
        auto& t = iteration.read.threads[r];
        const auto& tTimer = iterationTimer.read.threads[r];
        t.totalTime = tTimer.endTime - tTimer.startTime;
        addStat(iteration.read.threadTimes, t.totalTime);

        const size_t callCount = tTimer.calls.size();
        t.call.resize(callCount);

        for (auto c = 0; c < callCount; ++c) {
          t.call[c] = tTimer.calls[c].endTime - tTimer.calls[c].startTime;
          addStat(t.callTimes, t.call[c]);
        }

        normalize(t.callTimes, callCount);
        addStat(iteration.read.callTimes, t.callTimes);
      }
      for (auto w = 0; w < writeSize; ++w) {
        auto& t = iteration.write.threads[w];
        const auto& tTimer = iterationTimer.write.threads[w];
        t.totalTime = tTimer.endTime - tTimer.startTime;
        addStat(iteration.write.threadTimes, t.totalTime);

        const size_t callCount = tTimer.calls.size();
        t.call.resize(callCount);

        for (auto c = 0; c < callCount; ++c) {
          t.call[c] = tTimer.calls[c].endTime - tTimer.calls[c].startTime;
          addStat(t.callTimes, t.call[c]);
        }

        normalize(t.callTimes, callCount);
        addStat(iteration.write.callTimes, t.callTimes);
      }

      normalize(iteration.read.threadTimes,  readSize);
      normalize(iteration.write.threadTimes, writeSize);
      normalize(iteration.read.callTimes,    readSize);
      normalize(iteration.write.callTimes,   writeSize);

      addStat(read.threadTimes,  iteration.read.threadTimes);
      addStat(write.threadTimes, iteration.write.threadTimes);
      addStat(read.callTimes,    iteration.read.callTimes);
      addStat(write.callTimes,   iteration.write.callTimes);
    }

    normalize(iterationTimes,    iterationCount);
    normalize(read.asyncTimes,   iterationCount);
    normalize(write.asyncTimes,  iterationCount);
    normalize(read.threadTimes,  iterationCount);
    normalize(write.threadTimes, iterationCount);
    normalize(read.callTimes,    iterationCount);
    normalize(write.callTimes,   iterationCount);
  }
};





template <typename CharType, size_t N>
class FilledStringLiteral{
  CharType buffer[N]{};


  template <size_t I>
  inline static constexpr CharType fillWith(CharType Val) noexcept {
    return Val;
  }

  template <size_t ...I>
  constexpr explicit FilledStringLiteral(CharType Val, std::index_sequence<I...>) noexcept
      : buffer{ fillWith<I>(Val)... }{}

public:
  /*constexpr FilledStringLiteral(CharType Val) noexcept{
    for (auto& c : buffer)
      c = Val;

  }*/
  constexpr FilledStringLiteral(CharType Val) noexcept : FilledStringLiteral(Val, std::make_index_sequence<N>{}){}

  constexpr std::string_view view() const noexcept {
    return {buffer, N};
  }
  constexpr std::string_view view(size_t n) const noexcept {
    return {buffer, n};
  }
};

class IndentedOS{
  inline constexpr static FilledStringLiteral<char, 64> IndentString{' '};
  uint32_t indentCount = 0;
  const uint32_t spacesPerIndent = 2;
  std::ostream& outStream;
public:
  IndentedOS(std::ostream& OS) noexcept : outStream(OS){}
  IndentedOS(std::ostream& OS, uint32_t spacePerIndent) noexcept : outStream(OS), spacesPerIndent(spacePerIndent){}
  ~IndentedOS(){
    assert(indentCount == 0);
  }

  IndentedOS& beginIndent() noexcept {
    ++indentCount;
    return *this;
  }
  IndentedOS& endIndent() noexcept {
    --indentCount;
    return *this;
  }
  IndentedOS& newLine() noexcept {
    outStream << "\n" << IndentString.view(indentCount * spacesPerIndent);
    return *this;
  }

  template <typename T> requires(requires(T&& t){
    { outStream << std::forward<T>(t) } -> std::same_as<decltype(outStream)>;
  })
  IndentedOS& write(T&& t) noexcept {
    outStream << std::forward<T>(t);
    return *this;
  }
};


template <typename T>
IndentedOS& operator<<(IndentedOS& os, T&& t) noexcept requires(requires{
  { os.write(std::forward<T>(t)) } -> std::same_as<IndentedOS&>;
}){
  return os.write(std::forward<T>(t));
}

template <typename Rep>
IndentedOS& operator<<(IndentedOS& OS, std::chrono::duration<Rep> s){
  return OS << "\"" << s.count() << " s\"";
}
template <typename Rep>
IndentedOS& operator<<(IndentedOS& OS, std::chrono::duration<Rep, std::milli> ms){
  if (ms.count() >= 1000)
    return OS << duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(ms);
  return OS << "\""  << ms.count() << " ms\"";
}
template <typename Rep>
IndentedOS& operator<<(IndentedOS& OS, std::chrono::duration<Rep, std::micro> us){
  if (us.count() >= 1000)
    return OS << duration_cast<std::chrono::duration<double, std::milli>>(us);
  return OS << "\"" << us.count() << " us\"";
}
template <typename Rep>
IndentedOS& operator<<(IndentedOS& OS, std::chrono::duration<Rep, std::nano> ns){
  if (ns.count() >= 1000)
    return OS << duration_cast<std::chrono::duration<double, std::micro>>(ns);
  return OS << "\"" << ns.count() << " ns\"";
}




IndentedOS& operator<<(IndentedOS& os, IndentedOS&(*pFunction)(IndentedOS&) noexcept) noexcept {
  return pFunction(os);
}



IndentedOS& beginIndent(IndentedOS& os) noexcept{
  return os.beginIndent();
}
IndentedOS& endIndent(IndentedOS& os) noexcept{
  return os.endIndent();
}
IndentedOS& newLine(IndentedOS& os) noexcept{
  return os.newLine();
}

template <typename T, typename Alloc>
IndentedOS& operator<<(IndentedOS& os, const std::vector<T, Alloc>& vec) noexcept {
  os << "[ " << beginIndent << newLine;
  auto       begin = vec.begin();
  const auto end = vec.end() - 1;
  for (; begin != end; ++begin)
    os << *begin << "," << newLine;
  return os << *end << endIndent << newLine << "]";
}

IndentedOS& operator<<(IndentedOS& OS, const BasicTimerStats& stats){
  return OS << "{" << beginIndent << newLine
                    << "\"ave\": " << stats.ave << "," << newLine
                    << "\"min\": " << stats.min << "," << newLine
                    << "\"max\": " << stats.max << endIndent << newLine
            << "}";
}
IndentedOS& operator<<(IndentedOS& OS, const ExtendedTimerStats& stats){
  BasicTimerStats ave, min, max;
  ave.ave = stats.ave;
  ave.min = duration_cast<Duration>(stats.minAve);
  ave.max = duration_cast<Duration>(stats.maxAve);
  min.ave = stats.aveMin;
  min.max = stats.maxMin;
  min.min = stats.min;
  max.ave = stats.aveMax;
  max.min = stats.minMax;
  max.max = stats.max;

  return OS << "{" << beginIndent << newLine <<
                      "\"ave\": " << ave << "," << newLine <<
                      "\"min\": " << min << "," << newLine <<
                      "\"max\": " << max << endIndent << newLine
            << "}";
}
IndentedOS& operator<<(IndentedOS& OS, const ProcessIterationResult& stats){
  return OS << "{" << beginIndent << newLine
                   << "\"callTimes\": " << stats.callTimes << "," << newLine
                   << "\"threadTimes\": " << stats.threadTimes << "," << newLine
                   << "\"asyncTime\": " << stats.asyncTimes << endIndent << newLine
            << "}";
}
IndentedOS& operator<<(IndentedOS& OS, const ThreadTimerResult& stats) {
  return OS << "{" << beginIndent << newLine
                   << "\"totalTime\": " << stats.totalTime << "," << newLine
                   << "\"callTime\": "  << stats.callTimes << "," << newLine
                   << "\"calls\": "  << stats.call      << endIndent << newLine
            << "}";
}
IndentedOS& operator<<(IndentedOS& OS, const AsyncTimerResult& stats) {
  return OS << "{" << beginIndent << newLine
                   << "\"totalTime\": " << stats.totalTime << "," << newLine
                   << "\"threadTime\": " << stats.threadTimes << "," << newLine
                   << "\"callTime\": " << stats.callTimes   << "," << newLine
                   << "\"threads\": " << stats.threads     << endIndent << newLine
            << "}";
}
IndentedOS& operator<<(IndentedOS& OS, const IterationTimerResult& stats) {
  Duration totalTime = Duration::zero();
  AsyncTimerResult read{};
  AsyncTimerResult write{};

  return OS << "{" << beginIndent << newLine
                   << "\"totalTime\": " << stats.totalTime << "," << newLine
                   << "\"write\": " << stats.write << "," << newLine
                   << "\"read\": " << stats.read << endIndent << newLine
            << "}";
}
IndentedOS& operator<<(IndentedOS& OS, const ProcessTimerResult& timerResults) {
  return OS << "{" << beginIndent << newLine
                   << "\"totalTime\": " << timerResults.totalTime << "," << newLine
                   << "\"iterationTime\": " << timerResults.iterationTimes << "," << newLine
                   << "\"write\": " << timerResults.write << "," << newLine
                   << "\"read\": " << timerResults.read << endIndent << newLine
                   //<< "\"read\": " << timerResults.read << "," << newLine
                   //<< "\"iterations\": " << timerResults.iterations << endIndent << newLine
            << "}";
}





#define TIME_FUNCTION(timer_) ScopedTimer timer_RAII(timer_)


template <typename F>
void                    launchImpl(uint32_t threadCount, F&& func){
  TempSpan<std::thread> threads{threadCount};

  for (auto&& t : threads) {
    t = std::thread{ func };
  }

  for (auto&& t : threads) {
    t.join();
  }
}
template <typename F, typename C>
std::invoke_result_t<C> launchImpl(uint32_t threadCount, F&& func, C&& completionFunc){
  TempSpan<std::thread> threads{threadCount};

  for (auto&& t : threads) {
    t = std::thread{ func };
  }

  for (auto&& t : threads) {
    t.join();
  }

  return std::forward<C>(completionFunc)();
}

template <typename F>
std::future<void>                    launch(uint32_t threadCount, F&& func){
  return std::async(std::launch::async, launchImpl<F>, threadCount, std::forward<F>(func));
}
template <typename F, typename C>
std::future<std::invoke_result_t<C>> launch(uint32_t threadCount, F&& func, C&& completionFunc){
  return std::async(std::launch::async, launchImpl<F, C>, threadCount, std::forward<F>(func), std::forward<C>(completionFunc));
}






bool writeProc(std::string_view message, Buffer& buffer, BasicTimer& timer){

  /*if (buffer.lastMsgIndex.load(std::memory_order_acquire) != LastMessageIndexPlaceholder)
    return false;*/

  /*std::string thisMessage{"[ thread"};
  thisMessage += writeThread.id + " ]: writeId = { " + std::to_string(writeThread.writeCount++) + " }";*/

  TIME_FUNCTION(timer);

  const uint32_t messageStructSize = nextOffset(message);

  uint32_t writeOffset = 0;
  uint32_t nextOffset;
  Message* pMessage;

  do {
    nextOffset = writeOffset + messageStructSize;
    if (buffer.bufferSize < (nextOffset + offsetof(Message, nextOffset))) {
      //reinterpret_cast<Message*>(static_cast<std::byte*>(buffer.pBuffer) + writeOffset)->state = MessageState::Final;
      return false;
    }
    //nextOffset -= buffer.bufferSize * static_cast<bool>(buffer.bufferSize < nextOffset);
  } while(!buffer.nextWriteOffset.compare_exchange_strong(writeOffset, nextOffset, std::memory_order_acq_rel));


  pMessage = std::launder(new(static_cast<std::byte*>(buffer.pBuffer) + writeOffset) Message);
  pMessage->nextOffset.store(nextOffset, std::memory_order_release);
  pMessage->msgLength = uint32_t(message.length());
  std::memcpy(pMessage->pData, message.data(), pMessage->msgLength);


  //const uint32_t globalNextWrite = buffer.nextWriteOffset.load(std::memory_order_acquire);
  pMessage->state.store(MessageState::Written, std::memory_order_release);
  pMessage->state.notify_all();

  /*while (nextOffset  != NotYetWritten &&
         writeOffset != globalNextWrite &&
         buffer.writeCompleteOffset.compare_exchange_strong(writeOffset, nextOffset, std::memory_order_acq_rel)) {
    pMessage = reinterpret_cast<Message*>(static_cast<std::byte*>(buffer.pBuffer) + nextOffset);
    writeOffset = nextOffset;
    nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
  }

  buffer.writeCompleteOffset.notify_all();*/

  return true;
}

bool readProc(std::stringstream& outBuffer, Buffer& buffer, BasicTimer& timer){

  TIME_FUNCTION(timer);

  uint32_t readOffset;
  uint32_t nextOffset = 0;
  Message* pMessage;
  //MessageState readState = MessageState::Invalid;

  readOffset = static_cast<uint32_t>(-1);

  while (!buffer.nextReadOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel)) {
    if (buffer.bufferSize - readOffset <= offsetof(Message, pData))
      return false;

    pMessage   = reinterpret_cast<Message*>(static_cast<std::byte*>(buffer.pBuffer) + readOffset);
    //readState = pMessage->state.load();

    pMessage->state.wait(MessageState::Invalid);
    if (pMessage->state.load() == MessageState::Final)
      return false;
    //assert(readState != MessageState::Processed);

    nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
  }


  assert(readOffset != nextOffset);

  /*do {
    if (buffer.bufferSize == readOffset)
      return false;
    buffer.writeCompleteOffset.wait(readOffset);

    pMessage = reinterpret_cast<Message*>(static_cast<std::byte*>(buffer.pBuffer) + readOffset);
    nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);

    if (buffer.bufferSize < nextOffset)
      return false;

  } while(!buffer.nextReadOffset.compare_exchange_weak(readOffset, nextOffset, std::memory_order_acq_rel));*/

  outBuffer.write(pMessage->pData, pMessage->msgLength);

  pMessage->state = MessageState::Read;

  assert(readOffset < nextOffset);

#if defined(NDEBUG)
#define ASSERT_EVAL(...) __VA_ARGS__
#else
#define ASSERT_EVAL(...) assert(__VA_ARGS__)
#endif


  constexpr static auto checkIfRead = [](auto&& state){
    MessageState readState = MessageState::Read;
#if defined(NDEBUG)
    return state.compare_exchange_strong(readState, MessageState::Processed);
#else
    const bool result = state.compare_exchange_strong(readState, MessageState::Processed);
    //assert(result || readState == MessageState::Written);
    return result;
#endif

  };


  if (buffer.readCompleteOffset.compare_exchange_strong(readOffset, AcquireUnique, std::memory_order_acq_rel)){

    while (checkIfRead(pMessage->state)) {
      //readOffset = nextOffset;
      pMessage = reinterpret_cast<Message*>(static_cast<std::byte*>(buffer.pBuffer) + nextOffset);
      nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
      //assert(readOffset != nextOffset);
    }

    uint32_t releaseUnique = AcquireUnique;

    //readOffset = AcquireUnique;

    ASSERT_EVAL(buffer.readCompleteOffset.compare_exchange_strong(releaseUnique, nextOffset, std::memory_order_acq_rel));
    buffer.readCompleteOffset.notify_all();
  }

  //assert(readOffset < nextOffset || readOffset == AcquireUnique);

  /*if (buffer.readCompleteOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel)){
    while (checkIfValidState(pMessage->state)) {
      readOffset = nextOffset;
      pMessage = reinterpret_cast<Message*>(static_cast<std::byte*>(buffer.pBuffer) + readOffset);

      nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
      assert(readOffset != nextOffset);
      ASSERT_EVAL(buffer.readCompleteOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel));
    }
  }*/


  /*while (checkIfValidState(pMessage->state) &&
         buffer.readCompleteOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel)) {

    pMessage = reinterpret_cast<Message*>(static_cast<std::byte*>(buffer.pBuffer) + readOffset);

    //readOffset = nextOffset;

    nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
    assert(readOffset != nextOffset);
  }*/

  /*do {
    ((std::byte*&)pMessage) += pMessage->mSize;
    nextOffset += pMessage->mSize;
  } while(pMessage->mKind == MessageFullyProcessed);

  pMailbox->writeCompleteOffset.compare_exchange_strong(readOffset, nextOffset);

  reinterpret_cast<AgentMessage*>(pMailbox->pBuffer + readOffsetCopy)->mKind = MessageFullyProcessed;*/

  return true;
}


std::future<void> launchWrite(uint32_t threadCount, Buffer& buffer, AsyncTimer& timer){
  return std::async(std::launch::async, [&, threadCount](){
    TIME_FUNCTION(timer);
    TempSpan<std::thread> threads{threadCount};
    timer.threads.resize(threadCount);

    for (uint32_t i = 0; i < threadCount; ++i) {
      threads[i] = std::thread{ [&](ThreadTimer& threadTimer){

        TIME_FUNCTION(threadTimer);

        std::string          id = std::to_string(WriteThreadId++);
        std::atomic_uint32_t writeCount = 0;

        std::string          message{};

        do {
          message.assign("[ thread");
          message += id + " ]: writeId = { " + std::to_string(writeCount++) + " }";
          //std::this_thread::yield();
        } while (writeProc(message, buffer, threadTimer.calls.emplace_back()));
      }, std::ref(timer.threads[i]) };
    }

    for (auto&& t : threads) {
      t.join();
    }

    auto& state = ((Message*)((std::byte*)buffer.pBuffer + buffer.nextWriteOffset.load()))->state;
    state = MessageState::Final;
    state.notify_all();
  });
}
std::future<void> launchRead (uint32_t threadCount, Buffer& buffer, AsyncTimer& timer){
  return std::async(std::launch::async, [&, threadCount]{

    TIME_FUNCTION(timer);

    TempSpan<std::thread> threads{threadCount};

    for (uint32_t i = 0; i < threadCount; ++i) {
      threads[i] = std::thread{
          [&](ThreadTimer& threadTimer){

            TIME_FUNCTION(threadTimer);

            uint32_t          id = ReadThreadId++;
            uint32_t          readCount = 0;
            std::stringstream outBuffer{};

            outBuffer.write("\t", 1);
            while (readProc(outBuffer, buffer, threadTimer.calls.emplace_back())) {
              outBuffer.write(",\n\t", 3);
              readCount += 1;
            }
            if (readCount > 0)
              outBuffer << "\b\b\b";
            /*std::stringstream out;
            out << "[ reader" << id << " ]: messages[" << readCount << "] = { \n" << outBuffer.str() << "\n}\n\n";
            std::cout << out.str() << std::endl;*/
          }, std::ref(timer.threads[i]) };
    }

    for (auto&& t : threads) {
      t.join();
    }
  });
}

using namespace std::chrono_literals;

inline constexpr static uint32_t BufferLength     = 0x1 << 14;
inline constexpr static uint32_t ReadThreadCount  = 5;
inline constexpr static uint32_t WriteThreadCount = 5;
inline constexpr static uint32_t IterationCount   = 128;






void mainIndirect(char* bufferData, size_t N, IterationTimer& timer){

  TIME_FUNCTION(timer);

  std::memset(bufferData, '\0', N);

  Buffer sharedBuffer{
      .pBuffer = bufferData,
      .bufferSize = BufferLength,
      .readCompleteOffset  = 0,
      .nextReadOffset = 0,
      .nextWriteOffset = 0
  };

  /*auto readLaunch  = launch(ReadThreadCount, [&sharedBuffer]{ readProcess(sharedBuffer); });
  auto writeLaunch = launch(WriteThreadCount, [&sharedBuffer]{ writeProcess(sharedBuffer); }, [&sharedBuffer]{
    auto& buffer = sharedBuffer;
    auto& state = ((Message*)((std::byte*)sharedBuffer.pBuffer + sharedBuffer.nextWriteOffset.load()))->state;
    state = MessageState::Final;
    state.notify_all();
  });*/

  timer.write.threads.resize(WriteThreadCount);
  timer.read.threads.resize(ReadThreadCount);

  auto writeLaunch = launchWrite(WriteThreadCount, sharedBuffer, timer.write);
  auto readLaunch =  launchRead(ReadThreadCount,  sharedBuffer,  timer.read);


  auto writeResult = writeLaunch.wait_for(4s);
  auto readResult  = readLaunch.wait_for(4s);


  if (writeResult == std::future_status::timeout || readResult == std::future_status::timeout) {
    throw std::system_error(0x000005B4, std::system_category());
  }
}




struct Bytes{
  uint64_t byteCount;
};

Bytes bytes(uint64_t b) noexcept{
  return {b};
}

IndentedOS& operator<<(IndentedOS& os, Bytes b) {

  enum tag{
    B,
    KB,
    MB,
    GB
  };

  uint64_t byteCount = b.byteCount;
  tag siTag = B;

  double byteTotal = b.byteCount;
  if (byteCount >= 1024) {
    byteTotal /= 1024;
    byteCount >>= 10;
    siTag = KB;
  }
  if (byteCount >= 1024) {
    byteTotal /= 1024;
    byteCount >>= 10;
    siTag = MB;
  }
  if (byteCount >= 1024) {
    byteTotal /= 1024;
    byteCount >>= 10;
    siTag = GB;
  }
  switch (siTag){
    case B:
      return os << "\"" << b.byteCount << " B\"";
    case KB:
      if (b.byteCount & (1024 - 1))
        return os << "\"" << byteTotal << " KB\"";
      else
        return os << "\"" << byteCount << " KB\"";
    case MB:
      if (b.byteCount & ((1024 * 1024) - 1))
        return os << "\"" << byteTotal << " MB\"";
      else
        return os << "\"" << byteCount << " MB\"";
    case GB:
      if (b.byteCount & ((1024 * 1024 * 1024) - 1))
        return os << "\"" << byteTotal << " GB\"";
      else
        return os << "\"" << byteCount << " GB\"";
  }
}

inline constexpr static size_t EnvBufferLength = 256;

int main(){

  char asanPath[EnvBufferLength]{};
  size_t envLength;

  if (auto err = getenv_s(&envLength, asanPath, "ASAN_OPTIONS")){
    char errorBuffer[256];
    auto strerror_result = strerror_s(errorBuffer, err);
    assert(!strerror_result);
    std::abort();
  }

  std::cout << "ASAN_OPTIONS=\"" << asanPath << "\"" << std::endl;





  auto* bufferData = (char*)malloc(BufferLength);
  bufferData[BufferLength] = 'A';
  bufferData[-10] = 'B';

  std::tm cTime{};
  std::tm* const timeStamp = &cTime;
  std::time_t timeType = std::time(nullptr);
  if (auto result = localtime_s(timeStamp, &timeType)) {
    char errString[256]{};
    strerror_s(errString, result);
    std::cerr << errString << std::endl;
    std::terminate();
  }




  ProcessTimer timer;
  timer.iterations.resize(IterationCount);


  // Body
  {
    TIME_FUNCTION(timer);

    for (unsigned i = 0; i < IterationCount; ++i)
      mainIndirect(bufferData, BufferLength, timer.iterations[i]);
  }

  free(bufferData);

  bufferData[0] = 'B';

  std::stringstream fileName;
  fileName << "mailboxBenchmark" << std::put_time(timeStamp, "-%d%b%Y-%Hh%Mm%Ss") << ".json";
  std::string fileNameString = fileName.str();

  std::ofstream resultsLog(fileNameString);
  assert(resultsLog.is_open());

  IndentedOS out(resultsLog);




  ProcessTimerResult timerResults(timer);

  out << "{" << beginIndent << newLine;
  out << R"("timeStamp": ")"    << std::put_time(timeStamp, "%c %Z") << "\"," << newLine;
  out << std::setprecision(4);
  out << "\"bufferLength\": "   << bytes(BufferLength) << ","   << newLine;
  out << "\"readThreads\": "    << ReadThreadCount     << ","   << newLine;
  out << "\"writeThreads\": "   << WriteThreadCount    << ","   << newLine;
  out << "\"iterationCount\": " << IterationCount      << ","   << newLine;
  out << "\"results\": "        << timerResults        << endIndent << newLine;
  out << "}";

  resultsLog.flush();

  if (!resultsLog) {
    std::cerr << "FILE FAILURE :(" << std::endl;
    resultsLog.close();
    std::terminate();
  }

  resultsLog.close();



  return 0;
}