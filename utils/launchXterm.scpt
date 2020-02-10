set Servers to paragraphs of (do shell script "/bin/cat
$HOME/sosp19/deploy/livenodes_1")

if application "iTerm" is running then
  tell application "iTerm"
    create window with default profile
    tell current tab of current window
      select
      tell current session

        -- make the window fullscreen
        tell application "System Events" to key code 36 using command down
        split horizontally with default profile

        set num_hosts to count of Servers
        repeat with n from 1 to num_hosts
          if n - 1 is (round (num_hosts / 2)) then
            -- move to lower split
            tell application "System Events" to keystroke "]" using command down
          else if n > 1 then
            -- split vertically
            tell application "System Events" to keystroke "d" using command down
          end if
          delay 1
          write text "ssh -i $HOME/disaggregatedblockchain.pem cc@" & (item n of
Servers)
        end repeat
      end tell
    end tell
  end tell
else
  activate application "iTerm"

end if
