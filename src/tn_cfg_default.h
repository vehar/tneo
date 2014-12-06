/*******************************************************************************
 *
 * TNeoKernel: real-time kernel initially based on TNKernel
 *
 *    TNKernel:                  copyright © 2004, 2013 Yuri Tiomkin.
 *    PIC32-specific routines:   copyright © 2013, 2014 Anders Montonen.
 *    TNeoKernel:                copyright © 2014       Dmitry Frank.
 *
 *    TNeoKernel was born as a thorough review and re-implementation of
 *    TNKernel. The new kernel has well-formed code, inherited bugs are fixed
 *    as well as new features being added, and it is tested carefully with
 *    unit-tests.
 *
 *    API is changed somewhat, so it's not 100% compatible with TNKernel,
 *    hence the new name: TNeoKernel.
 *
 *    Permission to use, copy, modify, and distribute this software in source
 *    and binary forms and its documentation for any purpose and without fee
 *    is hereby granted, provided that the above copyright notice appear
 *    in all copies and that both that copyright notice and this permission
 *    notice appear in supporting documentation.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE DMITRY FRANK AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DMITRY FRANK OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *    THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/**
 * \file
 *
 * TNeoKernel default configuration file, to be copied as `tn_cfg.h`.
 *
 * This project is intended to be built as a library, separately from main
 * project (although nothing prevents you from bundling things together, if you
 * want to).
 *
 * There are various options available which affects API and behavior of the
 * kernel. But these options are specific for particular project, and aren't
 * related to the kernel itself, so we need to keep them separately.
 *
 * To this end, file `tn.h` (the main kernel header file) includes `tn_cfg.h`,
 * which isn't included in the repository (even more, it is added to
 * `.hgignore` list actually). Instead, default configuration file
 * `tn_cfg_default.h` is provided, and when you just cloned the repository, you
 * might want to copy it as `tn_cfg.h`. Or even better, if your filesystem
 * supports symbolic links, copy it somewhere to your main project's directory
 * (so that you can add it to your VCS there), and create symlink to it named
 * `tn_cfg.h` in the TNeoKernel source directory, like this:
 *
 *     $ cd /path/to/tneokernel/src
 *     $ cp ./tn_cfg_default.h /path/to/main/project/lib_cfg/tn_cfg.h
 *     $ ln -s /path/to/main/project/lib_cfg/tn_cfg.h ./tn_cfg.h
 *
 * Default configuration file contains detailed comments, so you can read them
 * and configure behavior as you like.
 */

#ifndef _TN_CFG_DEFAULT_H
#define _TN_CFG_DEFAULT_H


/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- some defaults depend on architecture, so we should include
//   `tn_arch_detect.h`
#include "arch/tn_arch_detect.h"


/*******************************************************************************
 *    USER-DEFINED OPTIONS
 ******************************************************************************/

/**
 * This option enables run-time check which ensures that build-time options for
 * the kernel match ones for the application.
 *
 * Without this check, it is possible that you change your `tn_cfg.h` file, and
 * just rebuild your application without rebuilding the kernel. Then,
 * application would assume that kernel behaves accordingly to `tn_cfg.h` which
 * was included in the application, but this is actually not true: you need to
 * rebuild the kernel for changes to take effect.
 *
 * With this option turned on, if build-time configurations don't match, you
 * will get run-time error (`_TN_FATAL_ERROR()`) inside `tn_sys_start()`, which
 * is much more informative than weird bugs caused by configuration mismatch.
 *
 * <b>Note</b>: turning this option on makes sense if only you use TNeoKernel
 * as a separate library. If you build TNeoKernel together with the
 * application, both the kernel and the application always use the same
 * `tn_cfg.h` file, therefore this option is useless.
 *
 * \attention If this option is on, your application must include the file
 * `tn_app_check.c`.
 */
#ifndef TN_CHECK_BUILD_CFG
#  define TN_CHECK_BUILD_CFG        1
#endif


/**
 * Number of priorities that can be used by application, plus one for idle task
 * (which has the lowest priority). This value can't be higher than
 * architecture-dependent value `#TN_PRIORITIES_MAX_CNT`, which typically
 * equals to width of `int` type. So, for 32-bit systems, max number of
 * priorities is 32.
 *
 * But usually, application needs much less: I can imagine **at most** 4-5
 * different priorities, plus one for the idle task.
 *
 * Do note also that each possible priority level takes RAM: two pointers for
 * linked list and one `short` for time slice value, so on 32-bit system it
 * takes 10 bytes. So, with default value of 32 priorities available, it takes
 * 320 bytes. If you set it, say, to 5, you save `270` bytes, which might be
 * notable.
 *
 * Default: `#TN_PRIORITIES_MAX_CNT`.
 */
#ifndef TN_PRIORITIES_CNT
#  define TN_PRIORITIES_CNT      TN_PRIORITIES_MAX_CNT
#endif

/**
 * Enables additional param checking for most of the system functions.
 * It's surely useful for debug, but probably better to remove in release.
 * If it is set, most of the system functions are able to return two additional
 * codes:
 *
 *    * `#TN_RC_WPARAM` if wrong params were given;
 *    * `#TN_RC_INVALID_OBJ` if given pointer doesn't point to a valid object.
 *      Object validity is checked by means of the special ID field of type
 *      `enum #TN_ObjId`.
 *
 * @see `enum #TN_ObjId`
 */
#ifndef TN_CHECK_PARAM
#  define TN_CHECK_PARAM         1
#endif

/**
 * Allows additional internal self-checking, useful to catch internal
 * TNeoKernel bugs as well as illegal kernel usage (e.g. sleeping in the idle 
 * task callback). Produces a couple of extra instructions which usually just
 * causes debugger to stop if something goes wrong.
 */
#ifndef TN_DEBUG
#  define TN_DEBUG               0
#endif

/**
 * Whether old TNKernel names (definitions, functions, etc) should be
 * available.  If you're porting your existing application written for
 * TNKernel, it is definitely worth enabling.  If you start new project with
 * TNeoKernel from scratch, it's better to avoid old names.
 */
#ifndef TN_OLD_TNKERNEL_NAMES
#  define TN_OLD_TNKERNEL_NAMES  1
#endif

/**
 * Whether mutexes API should be available
 */
#ifndef TN_USE_MUTEXES
#  define TN_USE_MUTEXES         1
#endif

/**
 * Whether mutexes should allow recursive locking/unlocking
 */
#ifndef TN_MUTEX_REC
#  define TN_MUTEX_REC           1
#endif

/**
 * Whether RTOS should detect deadlocks and notify user about them
 * via callback
 *
 * @see see `tn_callback_deadlock_set()`
 */
#ifndef TN_MUTEX_DEADLOCK_DETECT
#  define TN_MUTEX_DEADLOCK_DETECT  1
#endif

/**
 *
 * <i>Takes effect if only `#TN_DYNAMIC_TICK` is <B>not set</B></i>.
 *
 * Number of "tick" lists of timers, must be a power or two; minimum value:
 * `2`; typical values: `4`, `8` or `16`.
 *
 * Refer to the \ref timers_implementation for details.
 *
 * Shortly: this value represents number of elements in the array of 
 * `struct TN_ListItem`, on 32-bit system each element takes 8 bytes.
 *
 * The larger value, the more memory is needed, and the faster
 * $(TN_SYS_TIMER_LINK) ISR works. If your application has a lot of timers
 * and/or sleeping tasks, consider incrementing this value; otherwise,
 * default value should work for you.
 */
#ifndef TN_TICK_LISTS_CNT
#  define TN_TICK_LISTS_CNT    8
#endif


/**
 * API option for `MAKE_ALIG()` macro.
 *
 * There is a terrible mess with `MAKE_ALIG()` macro: original TNKernel docs
 * specify that the argument of it should be the size to align, but almost all
 * ports, including "original" one, defined it so that it takes type, not size.
 *
 * But the port by AlexB implemented it differently
 * (i.e. accordingly to the docs)
 *
 * When I was moving from the port by AlexB to another one, do you have any
 * idea how much time it took me to figure out why do I have rare weird bug? :)
 *
 * So, available options:
 *
 *  * `#TN_API_MAKE_ALIG_ARG__TYPE`: 
 *             In this case, you should use macro like this: 
 *                `TN_MAKE_ALIG(struct my_struct)`
 *             This way is used in the majority of TNKernel ports.
 *             (actually, in all ports except the one by AlexB)
 *
 *  * `#TN_API_MAKE_ALIG_ARG__SIZE`:
 *             In this case, you should use macro like this: 
 *                `TN_MAKE_ALIG(sizeof(struct my_struct))`
 *             This way is stated in TNKernel docs
 *             and used in the port for dsPIC/PIC24/PIC32 by AlexB.
 */
#ifndef TN_API_MAKE_ALIG_ARG
#  define TN_API_MAKE_ALIG_ARG     TN_API_MAKE_ALIG_ARG__SIZE
#endif


/**
 * Whether profiler functionality should be enabled.
 * Enabling this option adds overhead to context switching and bumps
 * the size of `#TN_Task` structure by more than 120 bytes.
 *
 * @see `#tn_task_profiler_timing_get()`
 * @see `struct #TN_TaskTiming`
 */
#ifndef TN_PROFILER
#  define TN_PROFILER            0
#endif


/**
 * Whether software stack overflow check is enabled.  Enabling this option adds
 * small overhead to context switching and system tick processing
 * (`#tn_tick_int_processing()`). When stack overflow happens, the kernel calls
 * user-provided callback (see `#tn_callback_stack_overflow_set()`); if this
 * callback is undefined, the kernel calls `#_TN_FATAL_ERROR()`.
 *
 * This option is on by default for all architectures except PIC24/dsPIC, 
 * since this architecture has hardware stack pointer limit.
 */
#ifndef TN_STACK_OVERFLOW_CHECK
#  if defined(__TN_ARCH_PIC24_DSPIC__)
/*
 * On PIC24/dsPIC, we have hardware stack pointer limit, so, no need for
 * software check
 */
#     define TN_STACK_OVERFLOW_CHECK   0
#  else
/*
 * On all other architectures, software stack overflow check is ON by default
 */
#     define TN_STACK_OVERFLOW_CHECK   1
#  endif
#endif


/**
 * Whether the kernel should use \ref time_ticks__dynamic_tick scheme instead of
 * \ref time_ticks__static_tick.
 */
#ifndef TN_DYNAMIC_TICK
#  define TN_DYNAMIC_TICK        0
#endif





/*******************************************************************************
 *    PIC24/dsPIC-specific configuration
 ******************************************************************************/


/**
 * \def TN_P24_SYS_IPL
 * 
 * Maximum system interrupt priority. For details on system interrupts on
 * PIC24/dsPIC, refer to the section \ref pic24_interrupts 
 * "PIC24/dsPIC interrupts". 
 *
 * \attention you should also set `#TN_P24_SYS_IPL_STR` to appropriate value.
 *
 * Should be >= 1 and <= 6. Default: 4.
 *
 */

/**
 * \def TN_P24_SYS_IPL_STR
 *
 * The same as `#TN_P24_SYS_IPL` but should be set as string, for example:
 * \code{.c}
 * #define TN_P24_SYS_IPL_STR  "4"
 * \endcode
 *
 * It is used in assembly for ISR macro `tn_p24_soft_isr()`. 
 * I don't like that we have to keep two macros instead of just one, so if
 * anybody knows how to use integer value there, please let me know :)
 */

#ifndef TN_P24_SYS_IPL
//-- NOTE: the following two macros should correspond: they should specify
//   the same number, but in one case it is an integer, and in the second case
//   it is a string. If anyone knows how to write it so that we can specify
//   just a single macro, please let me know. :)
#  define TN_P24_SYS_IPL      4
#  define TN_P24_SYS_IPL_STR  "4"
#endif

#endif // _TN_CFG_DEFAULT_H


