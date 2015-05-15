#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>


int main() {
    struct utsname hostinfo;
    struct passwd *puserinfo  = NULL;
    struct group *pgroupinfo = NULL;
    struct rusage sysusage;
    time_t curTimeStamp;
    uid_t  userid;
    gid_t  groupid;
    openlog("sysinfo", LOG_NDELAY | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "sysinfo starts to run now.");

    if(uname(&hostinfo)) {
        syslog(LOG_ERR, "Error when getting host info: %d", errno);
        return 1;
    }

    syslog(LOG_INFO, "System Name: %s, Network Name: %s, Release: %s, Version: %s, Identifier: %s",
           hostinfo.sysname, hostinfo.nodename, hostinfo.release, hostinfo.version, hostinfo.machine);
    
    time(&curTimeStamp);
    syslog(LOG_INFO, "Current Time for this exe run: %s", ctime(&curTimeStamp));

    userid = getuid();
    puserinfo = getpwuid(userid);
    pgroupinfo = getgrgid(puserinfo->pw_gid);
    syslog(LOG_INFO, "Current User Name: %s, Group Name: %s, Shell Name: %s, Home: %s",
           puserinfo->pw_name, pgroupinfo->gr_name, puserinfo->pw_shell, puserinfo->pw_dir);

    
    if(getrusage(RUSAGE_SELF, &sysusage)) {
        syslog(LOG_ERR, "Error when getting system usage info: %d", errno);
        return 1;
    }

    syslog(LOG_INFO, "PID: %lld, Priority: %d, usertime: %lld.%lld, systemtime: %lld.%lld", 
           getpid(), getpriority(PRIO_PROCESS, getpid()), sysusage.ru_utime.tv_sec,
           sysusage.ru_utime.tv_usec, sysusage.ru_stime.tv_sec, sysusage.ru_stime.tv_usec);
    return 0;
}
