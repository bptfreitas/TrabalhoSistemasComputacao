#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <pthread.h>
#include <defs.h>
#include <produtor.h>
#include <cp1.h>
#include <cp2.h>
#include <cp3.h>
#include <consumidor.h>

volatile sig_atomic_t keep_running = 1;
volatile sig_atomic_t terminate_after_tasks = 0;

extern pthread_mutex_t sigterm_recv_lock;
extern int sigterm_recv;

void signal_handler(int sig) {
    switch (sig) {
        case SIGTERM:
            syslog(LOG_INFO, "SIGTERM received: Will terminate after completing all tasks.");

            pthread_mutex_lock (&sigterm_recv_lock);
            sigterm_recv = 1;
            pthread_mutex_unlock (&sigterm_recv_lock);
            
            keep_running = 0;
            break;
        case SIGKILL:
            syslog(LOG_INFO, "SIGKILL received: Terminating immediately.");
            exit(EXIT_FAILURE);
        default:
            syslog(LOG_WARNING, "Unhandled signal %d received.", sig);
    }
}

void daemonize() {
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) {
        perror("Failed to create a new session");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("Second fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        close(x);
    }

    openlog("matrix_daemon", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Daemon started.");
    signal(SIGTERM, signal_handler);
    signal(SIGKILL, signal_handler);

    buffer_t shared[4];

    for (int i = 0; i < 4; i++) {
        sem_init(&shared[i].full, 0, BUFFER_SIZE);
        sem_init(&shared[i].mutex, 0, 1);
        sem_init(&shared[i].empty, 0, 0);
        shared[i].in = 0;
        shared[i].out = 0;
    }

    pthread_t thread_id[TOTAL_THREADS];
    int index = 0;
    // Criandoa s threads
    for (int i = 0; i < N_PRODUTORES; i++) {
        pthread_create(&thread_id[index], NULL, produtor, &shared);
        index++;
    }

    for (int i = 0; i < N_CP1; i++) {
        pthread_create(&thread_id[index], NULL, cp1, &shared);
        index++;
    }

    for (int i = 0; i < N_CP2; i++) {
        pthread_create(&thread_id[index], NULL, cp2, &shared);
        index++;
    }

    for (int i = 0; i < N_CP3; i++) {
        pthread_create(&thread_id[index], NULL, cp3, &shared);
        index++;
    }

    for (int i = 0; i < N_CONSUMIDORES; i++) {
        pthread_create(&thread_id[index], NULL, consumidor, &shared);
        index++;
    }

    for (int i = 0; i < index; i++) {
        pthread_join(thread_id[i], NULL);
    }

    //Limpando semáforos
    for (int i = 0; i < 4; i++) {
        sem_destroy(&shared[i].full);
        sem_destroy(&shared[i].mutex);
        sem_destroy(&shared[i].empty);
    }

    syslog(LOG_INFO, "Daemon terminated.");
    closelog();
}

int main(int argc, char **argv) {
    daemonize();
}
