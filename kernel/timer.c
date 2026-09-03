// we have added/removed a virt timer, now adjust the phys timer accordingly
// caller must hold timerlock
// return 0 on success
static int adjust_sys_timer(void)
{
	unsigned long next = (unsigned long)-1; // upcoming firing time, to be determined

	for (int tt = 0; tt < N_TIMERS; tt++) {
		if (!timers[tt].handler)
			continue;

		if (timers[tt].elapseat < next) {
			if (timers[tt].elapseat < current_counter()) {
				/* timer expired, but handler not called? this could happen on
				   qemu when cpu is slow. call the handler here */
				if ((*timers[tt].handler)(
						tt,
						timers[tt].param,
						timers[tt].context) == 1) {

					// timer shall restart
					timers[tt].elapseat =
						current_counter() +
						TICKPERMS * timers[tt].delayms;

				} else {
					// timer shall not restart
					timers[tt].handler = 0;
				}
			} else {
				// give "next" a bit slack so current_counter()
				// won't exceed next before returning
				next = timers[tt].elapseat + 10 * 1000; // 10ms
			}
		}
	}

	BUG_ON(current_counter() > next);

	// no active virtual timers
	if (next == 0xFFFFFFFFFFFFFFFF)
		return 0;

	// TIMER_C1 is 32-bit; use low 32 bits of next expiration
	put32(TIMER_C1, (unsigned)next);

	return 0;
}


// return: timer id (>=0, <N_TIMERS) allocated. -1 on error
// the clock counter has 64bit, so we assume it won't wrap around
// in the current impl.
// "handler": callback, to be called in irq context
// NB: caller must hold & then release timerlock
static int ktimer_start_nolock(
	unsigned delayms,
	TKernelTimerHandler *handler,
	void *para,
	void *context)
{
	unsigned t;
	unsigned long cur;

	for (t = 0; t < N_TIMERS; t++) {
		if (timers[t].handler == 0)
			break;
	}

	if (t == N_TIMERS) {
		E("ktimer_start failed. # max timer reached");
		return -1;
	}

	cur = current_counter();

	BUG_ON(cur + TICKPERMS * delayms < cur);

	timers[t].handler = handler;
	timers[t].param = para;
	timers[t].context = context;
	timers[t].elapseat = cur + TICKPERMS * delayms;
	timers[t].delayms = delayms;

	adjust_sys_timer();

	return t;
}


// see above
// cannot be called from TKernelTimerHandler, which will have timerlock held
// thus, deadlock
int ktimer_start(
	unsigned delayms,
	TKernelTimerHandler *handler,
	void *para,
	void *context)
{
	int ret;

	acquire(&timerlock);

	ret = ktimer_start_nolock(
		delayms,
		handler,
		para,
		context
	);

	release(&timerlock);

	return ret;
}


// return 0 on okay, -1 if no such timer/handler,
// -2 if already fired (will clean anyway)
int ktimer_cancel(int t)
{
	unsigned long cur;

	if (t < 0 || t >= N_TIMERS)
		return -1;

	cur = current_counter();

	acquire(&timerlock);

	if (!timers[t].handler) {
		release(&timerlock);
		return -1;
	}

	if (timers[t].elapseat < cur) {
		timers[t].handler = 0;
		timers[t].context = 0;
		timers[t].param = 0;

		release(&timerlock);

		return -2;
	}

	timers[t].handler = 0;

	adjust_sys_timer();

	release(&timerlock);

	return 0;
}


// the irq handler for sys_timer
// called by irq.c
void sys_timer_irq(void)
{
	V("called");

	// timer1 must have pending match
	BUG_ON(!(get32(TIMER_CS) & TIMER_CS_M1));

	// clear timer1 match
	put32(TIMER_CS, TIMER_CS_M1);

	unsigned long cur = current_counter();
	int ret;

	acquire(&timerlock);

	for (int t = 0; t < N_TIMERS; t++) {

		TKernelTimerHandler *h = timers[t].handler;

		if (h == 0)
			continue;

		if (timers[t].elapseat <= cur) {

			// Quest 11: execute callback
			ret = (*h)(
				t,
				timers[t].param,
				timers[t].context
			);

			if (ret == 1) {

				// restart periodic virtual timer
				timers[t].elapseat =
					cur + TICKPERMS * timers[t].delayms;

				adjust_sys_timer();

			} else {

				timers[t].handler = 0;
			}
		}
	}

	adjust_sys_timer();

	release(&timerlock);
}

#endif
