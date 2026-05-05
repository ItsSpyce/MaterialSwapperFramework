#pragma once

class CriticalSectionLock {
public:
  explicit CriticalSectionLock(CRITICAL_SECTION* lock) : cs_(lock) {
    EnterCriticalSection(cs_);
  }

  ~CriticalSectionLock() {
    LeaveCriticalSection(cs_);
  }

  NOMOVE(CriticalSectionLock);
  NOCOPY(CriticalSectionLock);

private:
  CRITICAL_SECTION* cs_;
};