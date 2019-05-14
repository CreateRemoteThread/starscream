/*
 user_main for esp component
*/

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "packetforge.h"

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1
os_event_t    user_procTaskQueue[user_procTaskQueueLen];

#define CONFIG_DOGMA 1
#define CHANNEL_HOP_INTERVAL 60000
uint8_t channel = 7;
uint16_t seq_n = 0;
uint8_t packet_buffer[64];
static volatile os_timer_t chanhop_timer;

#define TYPE_MANAGEMENT       0x00
#define TYPE_CONTROL          0x01
#define TYPE_DATA             0x02
#define SUBTYPE_ASSOC_REQUEST 0x00
#define SUBTYPE_PROBE_RESPONSE 0x05
#define SUBTYPE_PROBE_REQUEST 0x04

// we "win" when we're able to try to connect
// to forge_ap.
uint8_t forge_ap[6] = {0x11,0x22,0x33,0x44,0x55,0x66};

/* ==============================================
 * Promiscous callback structures, see ESP manual
 * ============================================== */
 
struct RxControl {
    signed rssi:8;
    unsigned rate:4;
    unsigned is_group:1;
    unsigned:1;
    unsigned sig_mode:2;
    unsigned legacy_length:12;
    unsigned damatch0:1;
    unsigned damatch1:1;
    unsigned bssidmatch0:1;
    unsigned bssidmatch1:1;
    unsigned MCS:7;
    unsigned CWB:1;
    unsigned HT_length:16;
    unsigned Smoothing:1;
    unsigned Not_Sounding:1;
    unsigned:1;
    unsigned Aggregation:1;
    unsigned STBC:2;
    unsigned FEC_CODING:1;
    unsigned SGI:1;
    unsigned rxend_state:8;
    unsigned ampdu_cnt:8;
    unsigned channel:4;
    unsigned:12;
};
 
struct LenSeq {
    uint16_t length;
    uint16_t seq;
    uint8_t  address3[6];
};

struct sniffer_buf {
    struct RxControl rx_ctrl;
    uint8_t buf[36];
    uint16_t cnt;
    struct LenSeq lenseq[1];
};

struct sniffer_buf2{
    struct RxControl rx_ctrl;
    uint8_t buf[112];
    uint16_t cnt;
    uint16_t len;
};

static void ICACHE_FLASH_ATTR
promisc_cb(uint8_t *buf, uint16_t len)
{
  return;
}

void ICACHE_FLASH_ATTR
sniffer_system_init_done(void)
{
    wifi_set_channel(7);
    /*
    wifi_promiscuous_enable(0);
    wifi_set_promiscuous_rx_cb(promisc_cb);
    wifi_promiscuous_enable(1);
    */
}

static void user_procTask(os_event_t *events);
static void ICACHE_FLASH_ATTR user_procTask(os_event_t *events)
{
   char row;
   char col;
    char *pkt_buf = (char *)os_malloc(256);
      while(FAIL == uart_rx_one_char(&row)){system_soft_wdt_feed();};
      while(FAIL == uart_rx_one_char(&col)){system_soft_wdt_feed();};
      if(col < 0x20){
        // flush the buffer and try again;
        while(FAIL == uart_rx_one_char(&row)){system_soft_wdt_feed();};
        os_printf("-- STARSCREAM-TX: Chewing byte one and continuing --\r\n");
      }
      else
      {
      os_printf("CHARS: %02x %02x\r\n",row,col);
      char rc[2] = {0,0};
      rc[0] = row;
      rc[1] = col;
      uint16_t packetSize = proberesp(pkt_buf,"\xFF\xFF\xFF\xFF\xFF\xFF","\x22\x44\x66\x88\xFF\x00",0x1234,2,rc,7);
      // os_printf("SENDING: %d\r\n",packetSize);
      wifi_send_pkt_freedom(pkt_buf,packetSize-1,0);
      }
  system_os_post(user_procTaskPrio,0,0);
}


void ICACHE_FLASH_ATTR
user_init()
{
    uart_div_modify(0,UART_CLK_FREQ / 9600);
    os_printf("\r\n-- STARSCREAM-TX: You shouldn't be here :) --\r\n");
    wifi_set_opmode(STATION_MODE);
    system_init_done_cb(sniffer_system_init_done);
    wifi_set_channel(7);
    wifi_promiscuous_enable(1);
    system_os_task(user_procTask,user_procTaskPrio,user_procTaskQueue,user_procTaskQueueLen);
    system_os_post(user_procTaskPrio,0,0);
}
