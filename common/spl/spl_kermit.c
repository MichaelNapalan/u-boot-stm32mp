/*
 * (C) Copyright 2016-2020
 * 
 * VeriSilicon <hairun.wang@verisilicon.com>
 *
 * License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <spl.h>
#include <malloc.h>
#include <asm/u-boot.h>
#include <asm/utils.h>

#define BUF_SIZE 1024

DECLARE_GLOBAL_DATA_PTR;

static void set_kerm_bin_mode(unsigned long *addr);
static ulong kermit_load_serial_bin(ulong offset, int *psize);
static int k_recv(void);
static void bin_data_init(void);
static void bin_data_char(char new_char);
static void k_data_init(void);
static void k_data_save(void);
static void k_data_restore(void);
static void k_data_char(char new_char);
static char ktrans(char in);
static void os_data_save(void);
static void os_data_restore(void);
static void send_ack(int n);
static void send_nack(int n);
static int chk1(char *buffer);
static void s1_sendpacket(char *packet);
static void send_pad(void);
static void handle_send_packet(int n);


#define XON_CHAR        17
#define XOFF_CHAR       19
#define START_CHAR      0x01
#define ETX_CHAR	0x03
#define END_CHAR        0x0D
#define SPACE           0x20
#define K_ESCAPE        0x23
#define SEND_TYPE       'S'
#define DATA_TYPE       'D'
#define ACK_TYPE        'Y'
#define NACK_TYPE       'N'
#define BREAK_TYPE      'B'
#define tochar(x) ((char) (((x) + SPACE) & 0xff))
#define untochar(x) ((int) (((x) - SPACE) & 0xff))

#define SEND_DATA_SIZE  20

typedef struct
{
    void (*os_data_init)(void);
    void (*os_data_char)(char new_char);
    int os_data_state;
    int os_data_state_saved;
    char *os_data_addr;
    char *os_data_addr_saved;
    char *bin_start_address;
    char a_b[24];

    char his_eol;        /* character he needs at end of packet */
    int  his_pad_count;  /* number of pad chars he needs */
    char his_pad_char;   /* pad chars he needs */
    char his_quote;      /* quote chars he'll use */
    int k_data_escape;
    int k_data_escape_saved;

    char send_parms[SEND_DATA_SIZE];
    char *send_ptr;
}kermit_data_t;

#define	gd_os_data_init		(((kermit_data_t *)(gd->spl_kermit_data))->os_data_init)
#define	gd_os_data_char		(((kermit_data_t *)(gd->spl_kermit_data))->os_data_char)
#define	gd_os_data_state	(((kermit_data_t *)(gd->spl_kermit_data))->os_data_state)
#define	gd_os_data_state_saved	(((kermit_data_t *)(gd->spl_kermit_data))->os_data_state_saved)
#define	gd_os_data_addr		(((kermit_data_t *)(gd->spl_kermit_data))->os_data_addr)
#define	gd_os_data_addr_saved	(((kermit_data_t *)(gd->spl_kermit_data))->os_data_addr_saved)
#define	gd_bin_start_address	(((kermit_data_t *)(gd->spl_kermit_data))->bin_start_address)
#define	gd_a_b			(((kermit_data_t *)(gd->spl_kermit_data))->a_b)
#define	gd_his_eol		(((kermit_data_t *)(gd->spl_kermit_data))->his_eol)
#define	gd_his_pad_count	(((kermit_data_t *)(gd->spl_kermit_data))->his_pad_count)
#define	gd_his_pad_char		(((kermit_data_t *)(gd->spl_kermit_data))->his_pad_char)
#define	gd_his_quote		(((kermit_data_t *)(gd->spl_kermit_data))->his_quote)
#define	gd_k_data_escape	(((kermit_data_t *)(gd->spl_kermit_data))->k_data_escape)
#define	gd_k_data_escape_saved	(((kermit_data_t *)(gd->spl_kermit_data))->k_data_escape_saved)
#define	gd_send_parms		(((kermit_data_t *)(gd->spl_kermit_data))->send_parms)
#define	gd_send_ptr		(((kermit_data_t *)(gd->spl_kermit_data))->send_ptr)

int spl_kermit_init(void)
{
	gd->spl_kermit_data = malloc(sizeof(kermit_data_t));
	if (!gd->spl_kermit_data)
	{
		printf("spl_kermit_init: malloc failed\n");
		return -1;
	}

	memset(gd->spl_kermit_data, '\0', sizeof(kermit_data_t));

	return 0;
}

int spl_kermit_load_binary(ulong offset, ulong offset_copy)
{
	ulong addr;
	int load_baudrate, current_baudrate;
	int rcode = 0;
	int size;

    load_baudrate = current_baudrate = gd->baudrate;
    printf("## Ready for binary (kermit) download "
           "to 0x%08lX at %d bps...\n",
           offset,
           load_baudrate);

    addr = kermit_load_serial_bin(offset, &size);

    if (addr == ~0) {
        load_addr = 0;
        printf("## Binary (kermit) download aborted\n");
        rcode = 1;
    } else {
        printf("## Start Addr      = 0x%08lX\n", addr);
        load_addr = addr;
        if (offset_copy) {
            memcpy((void*)offset_copy, (void*)offset, size);
        }
    }
    return rcode;
}

#if 0
int spl_kermit_load_image(void)
{
	ulong offset = 0;
	ulong addr;
	int load_baudrate, current_baudrate;
	int rcode = 0;
	int size;
	const struct image_header *header;

    offset = (CONFIG_SYS_TEXT_BASE - sizeof(struct image_header));
    load_baudrate = current_baudrate = gd->baudrate;
    printf("## Ready for binary (kermit) download "
           "to 0x%08lX at %d bps...\n",
           offset,
           load_baudrate);

    addr = kermit_load_serial_bin(offset, &size);

    if (addr == ~0) {
        load_addr = 0;
        printf("## Binary (kermit) download aborted\n");
        rcode = 1;
    } else {
        printf("## Start Addr      = 0x%08lX\n", addr);
        load_addr = addr;
    }

	header = (struct image_header *)(CONFIG_SYS_TEXT_BASE -
					 sizeof(struct image_header));

	spl_parse_image_header(header);

    return rcode;
}
#endif

static void set_kerm_bin_mode(unsigned long *addr)
{
	gd_bin_start_address = (char *) addr;
	gd_os_data_init = bin_data_init;
	gd_os_data_char = bin_data_char;
}

static ulong kermit_load_serial_bin(ulong offset, int *psize)
{
	int size, i;

	set_kerm_bin_mode((ulong *) offset);
	size = k_recv();

	for (i=0; i<100; ++i) {
		if (tstc()) {
			(void) getc();
		}
		udelay(1000);
	}

	flush_cache(offset, size);

	printf("## Total Size      = 0x%08x = %d Bytes\n", size, size);

	*psize = size;

	return offset;
}

static int k_recv(void)
{
	char new_char;
	char k_state, k_state_saved;
	int sum;
	int done;
	int length;
	int n, last_n;
	int len_lo, len_hi;

	/* initialize some protocol parameters */
	gd_his_eol = END_CHAR;		/* default end of line character */
	gd_his_pad_count = 0;
	gd_his_pad_char = '\0';
	gd_his_quote = K_ESCAPE;

	/* initialize the k_recv and k_data state machine */
	done = 0;
	k_state = 0;
	k_data_init();
	k_state_saved = k_state;
	k_data_save();
	n = 0;				/* just to get rid of a warning */
	last_n = -1;

	/* expect this "type" sequence (but don't check):
	   S: send initiate
	   F: file header
	   D: data (multiple)
	   Z: end of file
	   B: break transmission
	 */

	/* enter main loop */
	while (!done) {
		/* set the send packet pointer to begining of send packet parms */
		gd_send_ptr = gd_send_parms;

		/* With each packet, start summing the bytes starting with the length.
		   Save the current sequence number.
		   Note the type of the packet.
		   If a character less than SPACE (0x20) is received - error.
		 */

#if 0
		/* OLD CODE, Prior to checking sequence numbers */
		/* first have all state machines save current states */
		k_state_saved = k_state;
		k_data_save ();
#endif

		/* get a packet */
		/* wait for the starting character or ^C */
		for (;;) {
			switch (getc ()) {
			case START_CHAR:	/* start packet */
				goto START;
			case ETX_CHAR:		/* ^C waiting for packet */
				return (0);
			default:
				;
			}
		}
START:
		/* get length of packet */
		sum = 0;
		new_char = getc();
		if ((new_char & 0xE0) == 0)
			goto packet_error;
		sum += new_char & 0xff;
		length = untochar(new_char);
		/* get sequence number */
		new_char = getc();
		if ((new_char & 0xE0) == 0)
			goto packet_error;
		sum += new_char & 0xff;
		n = untochar(new_char);
		--length;

		/* NEW CODE - check sequence numbers for retried packets */
		/* Note - this new code assumes that the sequence number is correctly
		 * received.  Handling an invalid sequence number adds another layer
		 * of complexity that may not be needed - yet!  At this time, I'm hoping
		 * that I don't need to buffer the incoming data packets and can write
		 * the data into memory in real time.
		 */
		if (n == last_n) {
			/* same sequence number, restore the previous state */
			k_state = k_state_saved;
			k_data_restore();
		} else {
			/* new sequence number, checkpoint the download */
			last_n = n;
			k_state_saved = k_state;
			k_data_save();
		}
		/* END NEW CODE */

		/* get packet type */
		new_char = getc();
		if ((new_char & 0xE0) == 0)
			goto packet_error;
		sum += new_char & 0xff;
		k_state = new_char;
		--length;
		/* check for extended length */
		if (length == -2) {
			/* (length byte was 0, decremented twice) */
			/* get the two length bytes */
			new_char = getc();
			if ((new_char & 0xE0) == 0)
				goto packet_error;
			sum += new_char & 0xff;
			len_hi = untochar(new_char);
			new_char = getc();
			if ((new_char & 0xE0) == 0)
				goto packet_error;
			sum += new_char & 0xff;
			len_lo = untochar(new_char);
			length = len_hi * 95 + len_lo;
			/* check header checksum */
			new_char = getc();
			if ((new_char & 0xE0) == 0)
				goto packet_error;
			if (new_char != tochar((sum + ((sum >> 6) & 0x03)) & 0x3f))
				goto packet_error;
			sum += new_char & 0xff;
/* --length; */ /* new length includes only data and block check to come */
		}
		/* bring in rest of packet */
		while (length > 1) {
			new_char = getc();
			if ((new_char & 0xE0) == 0)
				goto packet_error;
			sum += new_char & 0xff;
			--length;
			if (k_state == DATA_TYPE) {
				/* pass on the data if this is a data packet */
				k_data_char (new_char);
			} else if (k_state == SEND_TYPE) {
				/* save send pack in buffer as is */
				*gd_send_ptr++ = new_char;
				/* if too much data, back off the pointer */
				if (gd_send_ptr >= &gd_send_parms[SEND_DATA_SIZE])
					--gd_send_ptr;
			}
		}
		/* get and validate checksum character */
		new_char = getc();
		if ((new_char & 0xE0) == 0)
			goto packet_error;
		if (new_char != tochar((sum + ((sum >> 6) & 0x03)) & 0x3f))
			goto packet_error;
		/* get END_CHAR */
		new_char = getc();
		if (new_char != END_CHAR) {
		  packet_error:
			/* restore state machines */
			k_state = k_state_saved;
			k_data_restore();
			/* send a negative acknowledge packet in */
			send_nack(n);
		} else if (k_state == SEND_TYPE) {
			/* crack the protocol parms, build an appropriate ack packet */
			handle_send_packet(n);
		} else {
			/* send simple acknowledge packet in */
			send_ack(n);
			/* quit if end of transmission */
			if (k_state == BREAK_TYPE)
				done = 1;
		}
	}
	return ((ulong) gd_os_data_addr - (ulong) gd_bin_start_address);
}

static void bin_data_char(char new_char)
{
	switch (gd_os_data_state) {
	case 0:					/* data */
		*gd_os_data_addr++ = new_char;
		break;
	}
}
static void bin_data_init(void)
{
	gd_os_data_state = 0;
	gd_os_data_addr = gd_bin_start_address;
}

static void k_data_init(void)
{
	gd_k_data_escape = 0;
	gd_os_data_init();
}

static void k_data_save(void)
{
	gd_k_data_escape_saved = gd_k_data_escape;
	os_data_save();
}

static void k_data_restore(void)
{
	gd_k_data_escape = gd_k_data_escape_saved;
	os_data_restore();
}

static void os_data_save(void)
{
	gd_os_data_state_saved = gd_os_data_state;
	gd_os_data_addr_saved = gd_os_data_addr;
}

static void os_data_restore(void)
{
	gd_os_data_state = gd_os_data_state_saved;
	gd_os_data_addr = gd_os_data_addr_saved;
}

static void k_data_char(char new_char)
{
	if (gd_k_data_escape) {
		/* last char was escape - translate this character */
		gd_os_data_char(ktrans(new_char));
		gd_k_data_escape = 0;
	} else {
		if (new_char == gd_his_quote) {
			/* this char is escape - remember */
			gd_k_data_escape = 1;
		} else {
			/* otherwise send this char as-is */
			gd_os_data_char(new_char);
		}
	}
}

static char ktrans(char in)
{
	if ((in & 0x60) == 0x40) {
		return (char) (in & ~0x40);
	} else if ((in & 0x7f) == 0x3f) {
		return (char) (in | 0x40);
	} else
		return in;
}

static void send_ack(int n)
{
	gd_a_b[0] = START_CHAR;
	gd_a_b[1] = tochar(3);
	gd_a_b[2] = tochar(n);
	gd_a_b[3] = ACK_TYPE;
	gd_a_b[4] = '\0';
	gd_a_b[4] = tochar(chk1(&gd_a_b[1]));
	gd_a_b[5] = gd_his_eol;
	gd_a_b[6] = '\0';
	s1_sendpacket(gd_a_b);
}

static void send_nack(int n)
{
	gd_a_b[0] = START_CHAR;
	gd_a_b[1] = tochar(3);
	gd_a_b[2] = tochar(n);
	gd_a_b[3] = NACK_TYPE;
	gd_a_b[4] = '\0';
	gd_a_b[4] = tochar(chk1(&gd_a_b[1]));
	gd_a_b[5] = gd_his_eol;
	gd_a_b[6] = '\0';
	s1_sendpacket(gd_a_b);
}

static int chk1(char *buffer)
{
	int total = 0;

	while (*buffer) {
		total += *buffer++;
	}
	return (int) ((total + ((total >> 6) & 0x03)) & 0x3f);
}

static void s1_sendpacket(char *packet)
{
	send_pad();
	while (*packet) {
		putc(*packet++);
	}
}

static void send_pad(void)
{
	int count = gd_his_pad_count;

	while (count-- > 0)
		putc(gd_his_pad_char);
}

static void handle_send_packet(int n)
{
	int length = 3;
	int bytes;

	/* initialize some protocol parameters */
	gd_his_eol = END_CHAR;		/* default end of line character */
	gd_his_pad_count = 0;
	gd_his_pad_char = '\0';
	gd_his_quote = K_ESCAPE;

	/* ignore last character if it filled the buffer */
	if (gd_send_ptr == &gd_send_parms[SEND_DATA_SIZE - 1])
		--gd_send_ptr;
	bytes = gd_send_ptr - gd_send_parms;	/* how many bytes we'll process */
	do {
		if (bytes-- <= 0)
			break;
		/* handle MAXL - max length */
		/* ignore what he says - most I'll take (here) is 94 */
		gd_a_b[++length] = tochar(94);
		if (bytes-- <= 0)
			break;
		/* handle TIME - time you should wait for my packets */
		/* ignore what he says - don't wait for my ack longer than 1 second */
		gd_a_b[++length] = tochar(1);
		if (bytes-- <= 0)
			break;
		/* handle NPAD - number of pad chars I need */
		/* remember what he says - I need none */
		gd_his_pad_count = untochar(gd_send_parms[2]);
		gd_a_b[++length] = tochar(0);
		if (bytes-- <= 0)
			break;
		/* handle PADC - pad chars I need */
		/* remember what he says - I need none */
		gd_his_pad_char = ktrans(gd_send_parms[3]);
		gd_a_b[++length] = 0x40;	/* He should ignore this */
		if (bytes-- <= 0)
			break;
		/* handle EOL - end of line he needs */
		/* remember what he says - I need CR */
		gd_his_eol = untochar(gd_send_parms[4]);
		gd_a_b[++length] = tochar(END_CHAR);
		if (bytes-- <= 0)
			break;
		/* handle QCTL - quote control char he'll use */
		/* remember what he says - I'll use '#' */
		gd_his_quote = gd_send_parms[5];
		gd_a_b[++length] = '#';
		if (bytes-- <= 0)
			break;
		/* handle QBIN - 8-th bit prefixing */
		/* ignore what he says - I refuse */
		gd_a_b[++length] = 'N';
		if (bytes-- <= 0)
			break;
		/* handle CHKT - the clock check type */
		/* ignore what he says - I do type 1 (for now) */
		gd_a_b[++length] = '1';
		if (bytes-- <= 0)
			break;
		/* handle REPT - the repeat prefix */
		/* ignore what he says - I refuse (for now) */
		gd_a_b[++length] = 'N';
		if (bytes-- <= 0)
			break;
		/* handle CAPAS - the capabilities mask */
		/* ignore what he says - I only do long packets - I don't do windows */
		gd_a_b[++length] = tochar(2);	/* only long packets */
		gd_a_b[++length] = tochar(0);	/* no windows */
		gd_a_b[++length] = tochar(94);	/* large packet msb */
		gd_a_b[++length] = tochar(94);	/* large packet lsb */
	} while (0);

	gd_a_b[0] = START_CHAR;
	gd_a_b[1] = tochar(length);
	gd_a_b[2] = tochar(n);
	gd_a_b[3] = ACK_TYPE;
	gd_a_b[++length] = '\0';
	gd_a_b[length] = tochar(chk1(&gd_a_b[1]));
	gd_a_b[++length] = gd_his_eol;
	gd_a_b[++length] = '\0';
	s1_sendpacket(gd_a_b);
}

