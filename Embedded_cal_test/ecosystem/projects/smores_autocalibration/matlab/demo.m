

%%

com = '/dev/tty.usbmodem1411';
s = Smores(com);

%%
s.magnetControl('left', 'on');
%%
s.magnetControl('top', 'off');
%%
s.moveDofPosition('pan', 0,50,0,0,0,0,2);
%%
s.magnetControl('top', 'off');
%%
s.moveDofPosition('left', 0,60,0,0,0,0,2);

%%
s.moveDofPosition('right', 0,60,0,0,0,0,2);
%%
s.initDof('right')
