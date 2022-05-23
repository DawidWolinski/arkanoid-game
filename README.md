## Arkanoid

This is a clone of Arkanoid, an arcade game released in 1986 by Taito Corporation where the player uses a paddle to bounce the ball and break the bricks on the map. It was written in C++ using SFML. 

### How to start
**Compiling**

In order to compile the source code it is necessary to install the SFML:
1. Download from: https://www.sfml-dev.org/download/sfml/2.5.1/. 
2. In the compiler properties add path to: 
  - SFML headers (i.e. C:\SFML-2.5.1\include)
  - SFML libraries (i.e. C:\SFML-2.5.1\bin)
3. In the compile properties link the application to the SFML .lib files. The required .lib files are: "sfml-graphics.lib", "sfml-window.lib", "sfml-system.lib" and "sfml-audio.lib". The files for debugging contain "-d" suffix (i.e. "sfml-graphics-d.lib").
4. The "src" directory already contains DLL files, however if you wish to use a different version of SFML than "Visual C++ 15 (2017) - 64-bit" then the DLL files will have to be replaced. 
5. Run the "main.cpp" file.

Official SFML installation guide can be found here: https://www.sfml-dev.org/tutorials/2.5/start-vc.php.
\
\
**Executable file**

Download the 'Release' folder (arkanoid-game/x64/Release) and open "Arkanoid.exe".

### How to play
Use left and right arrows (or A and D) to move the paddle and bounce the ball. Use spacebar to shoot when 'Laser' power-up is active or to release the ball when 'Catch' power-up is active. Other information about how to play can be found here: https://strategywiki.org/wiki/Arkanoid/Gameplay

### Content
The objective was to make a clone that is nearly identical to the original game. It contains all 33 levels, including the boss fight and other features such as power-ups. 

In the original Arkanoid, upon losing all health points the player could continue the game by tossing additional coins into the arcade machine. While it is not a good idea to do it on a modern computer, the screen with message about tossing the coin inside is still displayed to pay homage to the original game. However, to compensate this, there is an option to save the progress and continue playing later on. 

Currently the missing features which were present in 1986 version are enemies, as well as intro and outro cutscenes. They might be, however, added in the future.



### Screenshots
<p align="middle">
  <img src="https://user-images.githubusercontent.com/45266505/142075887-43036ddd-48ad-426c-b237-0b9168293075.png" width=40% height=40%>
  <img src="https://user-images.githubusercontent.com/45266505/142075993-9668e848-66e2-4f8b-86a7-64f181f890c5.png" width=40% height=40%>
  <img src="https://user-images.githubusercontent.com/45266505/169883606-46cae50b-b9e4-4136-bc49-e69324e7d0af.png" width=40% height=40%>
  <img src="https://user-images.githubusercontent.com/45266505/169883622-a265583f-0358-4b10-b975-0b5b7dcbf41e.png" width=40% height=40%>
  <img src="https://user-images.githubusercontent.com/45266505/169883638-1b97f306-82aa-4ebc-90fc-23f21ece7dbf.png" width=40% height=40%>
  <img src="https://user-images.githubusercontent.com/45266505/169883655-95420fd5-328f-43b9-9151-f389393e060a.png" width=40% height=40%>
  <img src="https://user-images.githubusercontent.com/45266505/169883663-17ad11ac-1e3f-4f05-9b4f-207e2e70a182.png" width=40% height=40%>
  <img src="https://user-images.githubusercontent.com/45266505/169883680-91af50e8-27ba-46df-8a10-14cd766e6f74.png" width=40% height=40%>
  <img src="https://user-images.githubusercontent.com/45266505/142076400-792f2de1-a5d3-4368-95d1-27b9efb611b0.png" width=40% height=40%>
  <img src="https://user-images.githubusercontent.com/45266505/142076467-78feb417-1a67-475c-96cf-df4916adb7e1.png" width=40% height=40%>
</p>
