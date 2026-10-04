#include "msg_override.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static int fails=0;
#define CHECK(c) do{ if(!(c)){printf("FAIL line %d: %s\n",__LINE__,#c);fails++;} }while(0)

static size_t mk(uint8_t* out,int be,uint32_t declared,const uint8_t* pl,size_t n){
  memset(out,0,0x44); out[0]=be?1:0;
  if(be){out[0x40]=declared>>24;out[0x41]=declared>>16;out[0x42]=declared>>8;out[0x43]=declared;}
  else  {out[0x43]=declared>>24;out[0x42]=declared>>16;out[0x41]=declared>>8;out[0x40]=declared;}
  memcpy(out+0x44,pl,n); return 0x44+n;
}
int main(void){
  uint8_t f[4096], d[0x400]; size_t len; int rc;
  /* texto "Hola" + 0xFD (con ñ=0x84, ¡=0x8D) */
  const uint8_t pl[]={0x28,0x86,0x4D,0x41,0xF7,0x84,0x8D,0xFD};
  size_t fs=mk(f,0,sizeof pl,pl,sizeof pl);
  memset(d,0xAA,sizeof d); rc=port_msg_decode_blob(f,fs,d,0x400,&len);
  CHECK(rc==PORT_MSG_DECODE_OK); CHECK(len==8); CHECK(memcmp(d,pl,8)==0); CHECK(d[8]==0xAA);
  /* big endian */
  fs=mk(f,1,sizeof pl,pl,sizeof pl);
  rc=port_msg_decode_blob(f,fs,d,0x400,&len); CHECK(rc==PORT_MSG_DECODE_OK&&len==8);
  /* sin terminador -> se añade */
  const uint8_t nt[]={0x28,0x86};
  fs=mk(f,0,2,nt,2); memset(d,0xAA,sizeof d);
  rc=port_msg_decode_blob(f,fs,d,0x400,&len); CHECK(rc==PORT_MSG_DECODE_OK&&len==3&&d[2]==0xFD);
  /* tamaño exactamente 0x3FF sin terminador: cabe (indice 0x3FF) */
  uint8_t big[0x500]; memset(big,0x28,sizeof big);
  fs=mk(f,0,0x3FF,big,0x3FF); rc=port_msg_decode_blob(f,fs,d,0x400,&len);
  CHECK(rc==PORT_MSG_DECODE_OK&&len==0x400&&d[0x3FF]==0xFD);
  /* 0x400 o mas -> truncado, terminador en 0x3FE */
  fs=mk(f,0,0x450,big,0x450); rc=port_msg_decode_blob(f,fs,d,0x400,&len);
  CHECK(rc==PORT_MSG_DECODE_TRUNCATED&&len==0x3FF&&d[0x3FE]==0xFD);
  /* malformados: dest intacto */
  memset(d,0xAA,sizeof d);
  CHECK(port_msg_decode_blob(f,0x43,d,0x400,&len)==PORT_MSG_DECODE_BAD);
  fs=mk(f,0,0,pl,sizeof pl); CHECK(port_msg_decode_blob(f,fs,d,0x400,&len)==PORT_MSG_DECODE_BAD);
  fs=mk(f,0,100,pl,sizeof pl); CHECK(port_msg_decode_blob(f,fs,d,0x400,&len)==PORT_MSG_DECODE_BAD);
  fs=mk(f,0,sizeof pl,pl,sizeof pl); f[0]=7; CHECK(port_msg_decode_blob(f,fs,d,0x400,&len)==PORT_MSG_DECODE_BAD);
  CHECK(port_msg_decode_blob(NULL,fs,d,0x400,&len)==PORT_MSG_DECODE_BAD);
  CHECK(d[0]==0xAA&&d[1]==0xAA);
  /* tabla */
  CHECK(strcmp(port_msg_resource_suffix(0x00000000),"NONE")==0);
  CHECK(strcmp(port_msg_resource_suffix(0x00000001),"Intro_0001")==0);
  CHECK(strcmp(port_msg_resource_suffix((0x1Du<<16)|113),"Menus_Merlee_IncreaseAttack")==0);
  CHECK(strcmp(port_msg_resource_suffix((0x1Du<<16)|0x58),"Menus_0058")==0);
  CHECK(strcmp(port_msg_resource_suffix((0x2Eu<<16)|135),"Credits_0087")==0);
  CHECK(port_msg_resource_suffix((0x2Eu<<16)|136)==NULL);
  CHECK(port_msg_resource_suffix(0x2Fu<<16)==NULL);
  CHECK(port_msg_resource_suffix(0xFFFFFFFFu)==NULL);
  /* todos los IDs de todas las secciones resuelven y empiezan por nombre valido */
  unsigned tot=0; for(unsigned s=0;s<0x2F;s++){ for(unsigned i=0;i<0x10000;i++){ const char*n=port_msg_resource_suffix((s<<16)|i); if(!n)break; CHECK(*n); tot++; } }
  CHECK(tot==8009); printf("entries=%u\n",tot);
  printf(fails?"FAILED %d\n":"ALL OK\n",fails); return fails!=0;
}
