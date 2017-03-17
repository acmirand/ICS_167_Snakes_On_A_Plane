# ICS_167_Snakes_On_A_Plane
ICS 167 - Snake Multiplayer Game.

# Group Members:
Ney Congjuico     ncongjui@uci.edu    12432102
Zachary Hart      zhhart@uci.edu      70953123
Daniel Lara       larad@uci.edu       49651280
Avelino Miranda   acmirand@uci.edu    16732033

# Command List
[----Client-side commands----]
clienttime - sends timeA to server, which is used to display latency after
  client receives the servertime command.
startgame - lets the server know that it can start the game
setname - allows users to set what name they would like to be displayed on the
  canvas, if left empty goes to default values (P1 and P2).
setdir - Sends the snake's new direction when player hits a key
  (0-down, 1-up, 2-left, 3-right)

[----Server-side commands----]
print - sends information to be printed on the client’s console (used for
   debugging)
ready - server sends back that there are now two connections established
begin - starts the game
p1posupdate - sends the position of player 1's snake
p2posupdate - sends the position of player 2's snake
updatep1score - renders player 1's new score on the canvas
updatep2score - renders player 2's new score on the canvas
sendfood - renders the new food location on the canvas
clearp1tail - clears player 1's previous tail on the canvas
clearp2tail - clears player 2's previous tail on the canvas
drawboard - either starts the game with a new board or restarts the game,
  once initialized calls drawBoard() function to render the gamestate.
player1name - sends the display name for player 1
player2name - sends the display name for player 2
servertime - servers sends their time, used for calculating and displaying the
  latency

# Participation log:
[----Milestone 1 log----]
Contributions mostly identified as commits on the GitHub repository:
https://github.com/acmirand/ICS_167_Snakes_On_A_Plane

The group had 2 meetings (1/27/17 and 2/1/17) where we all worked on
the code together but only one person was doing the commits. Both meetings were
attended by everyone in the group.

[----Milestone 2 log----]
Contributions mostly identified as commits on the GitHub repository:
https://github.com/acmirand/ICS_167_Snakes_On_A_Plane

The group had 5 successive meetings in total (2/13/17 to 2/17/17). Meetings
were mostly conducted using TeamViewer where we all worked on the same code.

Work breakdown:
Al - reference guy for the game logic since he wrote the javascript version of
   the game, helped debug the game loop on the server end
Zach - worked on getting client keyboard inputs (direction) to work, and
   translated those messages into the server
Daniel - worked on setting up the game loop, managed snake and wall collisions,
   snake growing updates and resets
Ney - worked on the display-side, getting food and scores to update, and
   rendering them to clients accordingly

[----Milestone 3 log---]
Contributions mostly identified as commits on the GitHub repository:
https://github.com/acmirand/ICS_167_Snakes_On_A_Plane

The group had 3 meetings in total (2/22/17, 2/24/17, and 2/25/17). Meetings
were mostly conducted using TeamViewer where we all worked on the same code.
Al couldn't make it to the 2/24 meeting.

Work breakdown:
Al - mostly helped with debugging the code and delay logic
Zach - attempted to calculate the NTP on both the client and server side
   (2/22/17 commits on github). Most of it we scratched, but it laid down the
   foundation for how we wanted to calculate the latency, listeners-wise.
Daniel - worked on calculating latency and getting the delay logic down. Worked
   on the mechanism to add the delay on the messages to send them out based on
   the delay, so now we're taking into account out-of-order messages.
Ney - worked on displaying and calculating latency. Attempted to add artificial
   latency using a queue and random generator (2/24/17 github commit) but was
   partly unfinished because it wasn't taking into account out-of-order messages.

[----Milestone 4 log---]
Contributions mostly identified as commits on the GitHub repository:
https://github.com/acmirand/ICS_167_Snakes_On_A_Plane
The group had 6 meetings in total (3/1/17, 3/4/17, 3/5/17, 3/6/17, 3/8/17, and
3/10/17). Meetings were mostly conducted using TeamViewer where we worked on
the same code. Ney couldn't make it to the 3/6/17 meeting and most of the
3/8/17 meeting.

Work breakdown:
Al - mostly helped with debugging code and the latency mitigation logic
Zach - worked on implementing the bucket synchronization technique on the server
   side. Mostly worked with Daniel to get the logic to work, and helped debug
   the code with regards to timestamps and syntax.
Daniel - worked on getting the bucket synchronization technique to work. Was the
   brains behind visualizing the entire process from the set, map, queue, and
   delays which helped us write the actual code for it. Worked on processing the
   messages on the client-side as well.
Ney - suggested to use bucket synchronization to mitigate latency. Mostly worked
   on helping Zach and Daniel debug the code with regards to timestamps and
   syntax.

This milestone was a bit slow, spent 3/1 and 3/4 just brainstorming ways on how
we were going to go about mitigating latency after two failed attempts to go at
it using prediction. The bulk of our problem were syntax problems on getting the
timestamps right.

[----Milestone 5 log---]
Contributions mostly identified as commits on the GitHub repository:
https://github.com/acmirand/ICS_167_Snakes_On_A_Plane
The group had 1 meeting in total (3/16/17). Meeting was conducted using TeamViewer
where we looked over Al's one-page summary and the clean-up he did on the code.

Work breakdown:
Al - worked on writing a majority of the 1-page summary, also worked on documenting
   the entirety of the code (all relevant javascript and c++ files)
Zach, Daniel, & Ney - edited and proof-read the 1-page summary and the
   documentation

All logs were written by Ney Congjuico.
