test -r ~/.alias && . ~/.alias
PS1='GRASS 6.4.1 (synthetic_5m):\w > '
PROMPT_COMMAND="'/Applications/GRASS-6.4.app/Contents/MacOS/etc/prompt.sh'"
export PATH="/Applications/GRASS-6.4.app/Contents/MacOS/bin:/Applications/GRASS-6.4.app/Contents/MacOS/scripts:/Users/colinbell/Library/GRASS/6.4/Modules/bin:/Library/GRASS/6.4/Modules/bin:/Library/Frameworks/GDAL.framework/unix/bin:/Library/Frameworks/PROJ.framework/Programs:.:/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/X11/bin:/usr/local/git/bin:/users/saramcmillan/work/RHESSys"
export HOME="/Users/colinbell"
export GRASS_SHELL_PID=$$
trap "echo \"GUI issued an exit\"; exit" SIGQUIT
