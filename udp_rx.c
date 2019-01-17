/**
 * @ingroup     examples
 * @{
 *
 * @file        udp_rx.c
 * @brief       UDP receiver thread
 *
 * @author      [list team members here]
 *
 * Github Link: [insert here]
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "thread.h"
#include "msg.h"
#include "net/gnrc.h"
#include "udp_rx.h"
#include "timex.h"
#include "mutex.h"
#include "random.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

//TODO
#define PRNG_INITVAL            0  //TODO: please use a unique int
#define SLEEP_MSG_STR           ""
#define SLEEP_MSG_LEN           0

#define SLEEP_INTERVAL_SECS     (0)
#define RAND_USLEEP_MIN         (0)
#define RAND_USLEEP_MAX         (1000000)

static char stack[THREAD_STACKSIZE_MAIN];
static msg_t msg_queue[8];



static void *_udp_rx(void *arg)
{
    udp_rx_args_t *udp_rx_args = (udp_rx_args_t *)arg; 
    
    //added myself
    mutex_t *mutex = udp_rx_args->mutex;
    uint32_t udp_port = udp_rx_args->udp_port;

    msg_t msg, reply;
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
    reply.content.value = -ENOTSUP;
    
    msg_init_queue(msg_queue,8);

    gnrc_netreg_entry_t me_reg = { .demux_ctx = udp_port, .target.pid = thread_getpid() };
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &me_reg); 

    /* initialize PRNG */
    random_init(PRNG_INITVAL);
    printf("PRNG initialized to current time: %d\n", PRNG_INITVAL);

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("udp_rx: data received\n");
                // TODO: if(msg size is valid and msg includes sleep string){
                gnrc_pktsnip_t* targetsnip = gnrc_pktsnip_search_type(msg.content.ptr, GNRC_NETTYPE_UNDEF);
                if (targetsnip->size == 5 && strncmp(targetsnip->data,"sleep",5)){
                    mutex_lock(mutex);
                    xtimer_sleep(4);
                    /* additional random sleep to reduce network collisions */
                    uint32_t interval = random_uint32_range(RAND_USLEEP_MIN, RAND_USLEEP_MAX);
                    xtimer_usleep(interval);
                    printf("Random interval: %d\n", interval );
                    mutex_unlock(mutex);
                }
                gnrc_pktbuf_release(msg.content.ptr);
                break; 
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("udp_rx_thr: data to send: ");
                gnrc_pktbuf_release(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
            case GNRC_NETAPI_MSG_TYPE_GET:
                msg_reply(&msg, &reply);
                break;     
            default:
                DEBUG("udp_rx_thr: received something unexpected");
                break;
        }
    }

    /* should never be reached */
    DEBUG("ERROR!\n");
    return 0;
}

kernel_pid_t udp_rx_init(void *args)
{
    // return udp_rx_pid;
    return thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN, 
      THREAD_CREATE_STACKTEST, _udp_rx, args, "UDP server");
}