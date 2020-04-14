#!/bin/bash
# The url of the current window in the foreground.
#
# author: andreasl

# assert that Google Chrome is selected
xdotool getactivewindow getwindowname | grep -q ' - Google Chrome$' || exit 1

window_id="$(xdotool getactivewindow)"
xdotool key --window "$window_id" "ctrl+l"
xdotool key --window "$window_id" "ctrl+c"
