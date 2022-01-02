#!/usr/bin/bash

# apt install inotify-tools

PROCESS_NAME=RinPhone
TRIGGER_CMD='make start'

$TRIGGER_CMD &

timestamp() {
    date +"%s"
}

LAST_TS=$(timestamp)

inotifywait -r \
    -m \
    --excludei '\.(gitignore)$' \
    --event modify \
    --event delete \
    app_helpers \
    controllers \
    filters \
    models \
    plugins \
    test \
    | while read base event file
    do
        NOW_TS=$(timestamp)

        echo $base $event $file $LAST_TS $NOW_TS

        if [ $(( $NOW_TS - $LAST_TS )) -lt 1 ]; then
            continue
        fi

        if [[ ${file} =~ .*\.(hpp|h|cc|cpp)$ ]]; then
            pkill ${PROCESS_NAME}
            $TRIGGER_CMD &
        fi

        LAST_TS=$(timestamp)
    done
