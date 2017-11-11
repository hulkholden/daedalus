#pragma once

#ifndef SYSTEM_CONDITION_H_
#define SYSTEM_CONDITION_H_

class Mutex;
struct Condition;

extern const double kTimeoutInfinity;

Condition* ConditionCreate();
void ConditionDestroy(Condition* cond);
void ConditionWait(Condition* cond, Mutex* mutex, double timeout);
void ConditionSignal(Condition* cond);

#endif  // SYSTEM_CONDITION_H_
