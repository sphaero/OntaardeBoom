#!/bin/sh
name=`basename $0`
dirname=
pid_file="/var/run/$name.pid"
stdout_log="$HOME/$name.log"
stderr_log="$HOME/$name.err"
cmd1="screen -S opi -L -d -m /dev/ttyACM0 9600"

build_firmware() {
    git pull
    ./build.sh ../Kerstboom022 >> "$stdout_log" 2>> "$stderr_log"
}

get_pid() {
    cat "$pid_file"
}

is_running() {
    screen -ls | grep -q opi
    #[ -f "$pid_file" ] && ps `get_pid` > /dev/null 2>&1
}


case "$1" in
    start)
    if is_running; then
        echo "Already started"
    else
        echo "Starting $name"
        #cd "$dir"
        #if [ -z "$user" ]; then
        #    sudo $cmd >> "$stdout_log" 2>> "$stderr_log" &
        #else
        #    sudo -u "$user" $cmd >> "$stdout_log" 2>> "$stderr_log" &
        #fi
        #echo $! > "$pid_file"
        $cmd1 >> "$stdout_log" 2>> "$stderr_log"
	if ! is_running; then
            echo "Unable to start, see $stdout_log and $stderr_log"
            exit 1
        fi
    fi
    ;;
    stop)
    if is_running; then
        echo -n "Stopping $name.."
        screen -r -S opi -X kill

        if is_running; then
            echo "Not stopped; may still be shutting down or shutdown may have failed"
            exit 1
        else
            echo "Stopped"
        fi
    else
        echo "Not running"
    fi
    ;;
    restart)
    $0 stop
    if is_running; then
        echo "Unable to stop, will not attempt to start"
        exit 1
    fi
    $0 start
    ;;
    status)
    if is_running; then
        $0 stop
    fi
    build_firmware
    $0 start
    ;;
    rebuild)
    if is_running; then
        echo "Running"
	$0 stop
    fi
    build_firmware
    ;;
    attach)
    if is_running; then
        echo "Use \"CTRL-A d\" to detach"
        echo "Press return to continue..."
        read p
        screen -r -S opi
    else
        echo "Not running"
    fi
    ;;
    *)
    echo "Usage: $0 {start|stop|restart|status|attach}"
    exit 1
    ;;
esac

exit 0
