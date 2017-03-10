# ICS_167_Snakes_On_A_Plane
ICS 167 - Snake Multiplayer Game.

Group Members:
Ney Congjuico     ncongjui@uci.edu    12432102
Zachary Hart      zhhart@uci.edu      70953123
Daniel Lara       larad@uci.edu       49651280
Avelino Miranda   acmirand@uci.edu    16732033

[----Milestone 1 log----]
Contributions mostly identified as commits on the GitHub repository:
https://github.com/acmirand/ICS_167_Snakes_On_A_Plane

However the group had 2 meetings (1/27/17 and 2/1/17) where we all worked on
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
on the mechanism to add the delay on the messages to send them out based on the
delay, so now we're taking into account out-of-order messages.
Ney - worked on displaying and calculating latency. Attempted to add artificial
latency using a queue and random generator (2/24/17 github commit) but was
partly unfinished because it wasn't taking into account out-of-order messages .

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
side. Mostly worked with Daniel to get the logic to work, and helped debug the
code with regards to timestamps and syntax.
Daniel - worked on getting the bucket synchronization technique to work. Was the
brains behind visualizing the entire process from the set, map, queue, and
delays which helped us write the actual code for it. Worked on processing the
messages on the client-side as well.
Ney - suggested to use bucket synchronization to mitigate latency. Mostly worked
on helping Zach and Daniel debug the code with regards to timestamps and syntax.

This milestone was a bit slow, spent 3/1 and 3/4 just brainstorming ways on how
we were going to go about mitigating latency after two failed attempts to go at
it using prediction. The bulk of our problem were syntax problems on getting the
timestamps right.

All logs were written by Ney Congjuico.
