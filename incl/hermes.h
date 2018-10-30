#ifndef HERMES_H
# define HERMES_H

# include "../libhermes/incl/libhermes.h"
# include "../binn/src/binn.h"

# include <pthread.h>
# include <signal.h>
# include <sys/socket.h>
# include <sys/poll.h>
# include <sys/time.h>
# include <arpa/inet.h>
# include <netinet/ip.h>
# include <netinet/in.h>
# include <netdb.h>
# include <netinet/ip_icmp.h>

# include "defined.h"

# define HERMES_VERSION "v0.0.1"

typedef struct sockaddr_in sockaddr_in;

typedef struct in_addr	t_ip4;

typedef struct sockaddr sockaddr;

typedef struct			s_ip4rng
{
	uint32_t			size;
	in_addr_t			start;
	in_addr_t			end;
}						t_ip4rng;

typedef struct			s_targetset
{
	uint32_t			total;
	uint32_t			ip_cnt;
	uint32_t			rng_cnt;
	t_node				*ips;						/*t_node tree containing t_ip4 structs*/
	t_node				*iprngs;					/*t_node tree containing t_ip4rng structs*/
}						t_targetset;

typedef struct			s_port
{
	uint16_t			port;
}						t_port;

typedef struct			s_prtrng
{
	uint16_t			size;
	uint16_t			start;
	uint16_t 			end;
}						t_prtrng;

typedef struct			s_portset
{
	uint16_t			total;
	uint16_t			port_cnt;
	uint16_t			rng_cnt;
	t_node				*ports;
	t_node				*prtrngs;
	uint16_t			*final;
}						t_portset;

typedef struct			s_optbitf
{
	uint64_t			no_rand_ports: 1;
	uint64_t			skip_hst_discov: 1;			/* Skip host discovery */
	uint64_t			do_list_scan: 1;			/* Do not send any probes, just do reverse dns resolution on target list */
	uint64_t			do_ping_scan: 1;			/* Do not send any probes, just do ping scan, and traceroute or OS Detection if specified, then quit */
	uint64_t			do_echo_discov: 1;			/* Send ICMP ECHO_REQUEST probe during host discovery */
	uint64_t			do_tstamp_discov: 1;		/* Send ICMP_TSTAMP probe during host discovery */
	uint64_t			do_nmask_discov: 1;			/* Send ICMP_MASKREQ probe during host discovery */
	uint64_t			do_syn_discov: 1;			/* Send TCP SYN probe during host discovery */
	uint64_t			do_ack_discov: 1;			/* Send TCP ACK probe during host discovery */
	uint64_t			do_udp_discov: 1;			/* Send UDP Datagram during host discovery */
	uint64_t			do_syn_scan: 1;
	uint64_t			do_null_scan: 1;
	uint64_t			do_ack_scan: 1;
	uint64_t			do_fin_scan: 1;
	uint64_t			do_xmas_scan: 1;
	uint64_t			do_udp_scan: 1;
	uint64_t			do_svd: 1;
	uint64_t 			do_osd: 1;
	uint64_t			do_iflist: 1;
	uint64_t			do_tracert: 1;				/* Perform a traceroute on hosts */
	uint64_t			do_bad_checksum: 1;
	uint64_t			output_xml: 1;
	uint64_t			output_norm: 1;
	uint64_t			show_only_open_ports: 1;
	uint64_t			apnd_file: 1;
}						t_optbitf;

typedef struct			s_ops
{
	t_optbitf			bitops;
	uint32_t			min_rtt_timeo;
	uint32_t			init_rtt_timeo;
	uint32_t			max_rtt_timeo;
//	uint32_t			icmp_ratelimit;
//	uint32_t			rst_ratelimit;
	uint32_t			host_timeo;
	uint32_t			scan_delay;
	uint32_t			max_scan_delay;
	uint32_t			min_packet_rate;
	uint32_t			max_packet_rate;
	uint32_t			max_retries;
	uint32_t			min_parallel;
	uint32_t			max_parallel;
	uint32_t			min_hostgroup;
	uint32_t			max_hostgroup;
	uint32_t			spoofed_srcaddr;
	uint16_t			spoofed_srcport;
	uint16_t			rpayload_len;
	uint8_t				fragment_mtu;
	uint8_t				ip_ttl;
	uint8_t				thread_count;
	uint8_t				verbose_level; /* Default 0, verbose 1, very verbose 2 TODO: possibe enum */
}						t_opts;

typedef struct			s_env
{
	t_opts				opts;
	t_portset			ports;
	t_portset			*syn_ports;
	t_portset			*ack_ports;
	t_portset			*udp_ports;
	void				*custom_payload;
}						t_env;

typedef struct			t_status
{
	uint8_t				running: 1;
	uint8_t				initilized: 1;
	uint8_t				work_requested: 1;
	uint8_t				has_work: 1;
}						t_stat;

typedef struct			s_worker
{
	t_stat				stat;
	struct sockaddr_in	sin;
	int					sock;
	t_targetset			targets;
	uint32_t			send_size;
	struct s_workerset	*set;
}						t_wrkr;

typedef struct			s_workerset
{
	uint32_t			cnt;
	uint32_t			wrking_cnt;
	nfds_t				maxfd;
	t_node				*wrkrs;
}						t_workerset;

typedef struct			s_portstat
{
	uint16_t			port;
	uint8_t				status;
}						t_portstat;

typedef struct			s_result
{
	t_ip4				ip;
	t_node				*port_stats;
}						t_result;

typedef struct			s_resultset
{
	uint32_t			byte_size;
	uint32_t			result_cnt;
	t_node				*results;
}						t_resultset;

typedef struct 			s_thread
{
	uint8_t				id;
	pthread_t			thread;
	struct s_thread_pool*pool;
	volatile bool		working;
	volatile bool		alive;
	uint16_t			amt;
}						t_thread;

typedef struct 			s_thread_pool
{
	uint16_t 			tcount;
	uint16_t			amt_working;
	uint16_t			reqest_amt;
	t_thread			*threads;
	t_resultset			*results;
	t_targetset			*work_pool;
	t_env				*env;
	pthread_mutex_t		amt_working_mtx;
	pthread_mutex_t		results_mtx;
	pthread_mutex_t		work_pool_mtx;
}						t_thread_pool;

typedef struct			s_worker_manager
{
	t_stat				stat;
	t_env				env;
	t_thread_pool		*tpool;
	t_resultset			results;
	t_targetset			targets;
	struct sockaddr_in	sin;
	int					sock;
	int					id;
}						t_wmgr;

typedef struct			s_manager
{
	t_stat				stat;
	t_env				env;
	t_thread_pool		*tpool;
	t_resultset			results;
	t_targetset			targets;
	t_targetset			thread_targets;
	t_workerset			workers;
	t_targetset			*exclude_targets;
	t_portset			*exclude_ports;
	FILE				*resume_file;
	FILE				*xml_file;
	FILE				*norm_file;
}						t_mgr;

t_mgr					*new_mgr(void);
t_env					*new_job(void);
t_opts					*new_opts(void);
t_result				*new_result(void);

t_ip4					*new_ip4(void);
int						ip4_cmp(void *left, void *right);
void					*ip4_min(t_node *tree);
in_addr_t				ip4_increment(in_addr_t ip, uint32_t increase);
in_addr_t				ip4_decrement(in_addr_t ip, uint32_t decrease);
uint32_t				ip4_diff(in_addr_t left, in_addr_t right);

t_ip4rng				*new_ip4range(void);
int						ip4rng_cmp(void *ipr_left, void *ipr_right);
int						ip4rng_overlap_cmp(void *left, void *right);
int						ip4_ip4rng_overlap_cmp(void *ip, void *iprng);
void					*ip4rng_min(t_node *tree);
t_node					*split_ip4rng_portions(t_ip4rng *data,
												 uint32_t splits);
t_ip4rng				*slice_ip4rng(t_ip4rng *src, uint32_t amt);

t_targetset				*new_targetset(void);

t_portstat				*new_portstat();
t_port					*new_port(void);
int						port_cmp(void *prt_left, void *prt_right);
void					*port_min(t_node *tree);

t_prtrng				*new_portrange(void);
t_portset				*new_portset();
int						portrng_cmp(void *rng_left, void *rng_right);
int						port_prtrng_overlap_cmp(void *prt, void *rng);
int						portrng_overlap_cmp(void *rng_left, void *rng_right);
void					*portrng_min(t_node *tree);

t_workerset				*new_workerset();
int						fd_idx(t_node *access);
t_wrkr					*new_worker(void);
int						worker_cmp(void *wrkr_left, void *wrkr_right);
void					*worker_min(t_node *tree);

int						sanity_check(t_mgr *mgr);
void					do_exclusions(t_mgr *mgr);
int						hermes_daemon(int port);
void					transfer_work(t_targetset *dst, t_targetset *src, uint32_t reqamt);

t_thread_pool				*init_threadpool(t_env *env, t_targetset *workpool, t_resultset *results);

int						worker_loop(t_wmgr *session);
int						manager_loop(t_mgr *mgr);
void					transfer_work(t_targetset *dst, t_targetset *src, uint32_t reqamt);

int						send_work(t_wrkr *worker);
void					run_scan(t_env *env, t_targetset *targets, t_resultset *res_ptr, pthread_mutex_t *res_mtx);

void					kill_threadpool(t_thread_pool *pool);
void					print_ip_struct(t_node *ip4);
void					print_iprng_struct(t_node *iprng);

binn					*binnify_resultset(t_resultset *set);

#endif
