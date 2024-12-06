#!/bin/bash

MONITOR_DIR="src/"
BUILD_DIR="build/"
SOLAR_SIMULATOR="$BUILD_DIR/solar-sim"
LOG_FILE="solar_sim.log"
LOCK_FILE="/tmp/solar_sim.lock"

if ! command -v inotifywait &> /dev/null; then
    echo "inotifywait missing"
    exit 1
fi

restart_gravity_sim() {
    if [ -f "$LOCK_FILE" ]; then
        echo "another build in progress."
        return
    fi

    touch "$LOCK_FILE"

    if pgrep -f "$SOLAR_SIMULATOR" > /dev/null; then
        echo "terminating existing process"
        pkill -f "$SOLAR_SIMULATOR"
    fi

    echo "changing to build directory and running make"
    (cd "$BUILD_DIR" && make)

    echo "executing exe"
    nohup "$SOLAR_SIMULATOR" > "$LOG_FILE" 2>&1 &

    rm -f "$LOCK_FILE"
}

echo "Monitoring changes in: $MONITOR_DIR"
inotifywait -m -r -e modify "$MONITOR_DIR" --format '%w%f %e' | while read FILE EVENT
do
    echo "detected changes: $FILE - Event: $EVENT"
    restart_gravity_sim
done

