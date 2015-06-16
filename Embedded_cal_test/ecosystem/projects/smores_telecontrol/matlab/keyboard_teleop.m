% keyboard_teleop.m
% Tarik Tosun, 2/20/15, University of Pennsylvania

function [] = move_fig()
% move figure with arrow keys.
S.fh = figure('units','pixels',...
              'position',[500 500 500 260],...
              'menubar','none',...
              'name','move_fig',...
              'numbertitle','off',...
              'resize','off',...
              'keypressfcn',@fh_kpfcn);
S.tx = uicontrol('style','text',...
                 'units','pixels',...
                 'position',[60 120 400 20],...
                 'fontweight','bold'); 
guidata(S.fh,S)             
function [] = fh_kpfcn(H,E)          
% Figure keypressfcn
S = guidata(H);
P = get(S.fh,'position');
%set(S.tx,'string',E.Key)
persistent state;
persistent T; % smores teleop
if isempty(T)
    T = Smores_Teleop;
end
% this first switch sets the state:
switch E.Key
    % magnets or wheels:
    case 'm'
        state.action = 'magnets'; state.face = []; state.direction = [];
    case 'w'
        state.action = 'wheels'; state.face = []; state.direction = [];
    % Faces / DOF
    case 't'
	if strcmp(state.action, 'magnets')
        	state.face = 'top';
	elseif strcmp(state.action, 'wheels')
		state.face = 'pan';
	end
	state.direction = [];
    case 'b'
        if strcmp(state.action, 'magnets')
            state.face = 'bottom';
        elseif strcmp(state.action, 'wheels')
            state.face = 'tilt';
        else
            assert(false);
        end
        state.direction = [];
    case 'r'
        state.face = 'right'; state.direction = [];
    case 'l'
        state.face = 'left'; state.direction = [];
    % directions:
    case 'rightarrow'
        if strcmp(state.action, 'magnets')
            state.direction = 'on';
        elseif strcmp(state.action, 'wheels')
            state.direction = 'ccw';
        else
            assert(false);
        end
    case 'leftarrow'
        if strcmp(state.action, 'magnets')
            state.direction = 'off';
        elseif strcmp(state.action, 'wheels')
            state.direction = 'cw';
        else
            assert(false);
        end
end
% Print to figure:
set(S.tx,'string',sprintf('%s, %s, %s',state.action, state.face, state.direction));
% Control the module:
if ~any([isempty(state.action), isempty(state.face), isempty(state.direction)])
    if strcmp(state.action, 'magnets')
        T.magnetControl(state.face, state.direction);
        disp('sent to mags');
    elseif strcmp(state.action, 'wheels')
        T.moveDofTime(state.face, state.direction, 50, 0.1);
        disp('sent');
    end
end

% switch E.Key
%     case 'rightarrow' 
%         set(S.fh,'pos',P+[5 0 0 0])
%         foo = 'wat';
%     case 'leftarrow'
%         set(S.fh,'pos',P+[-5 0 0 0])
%         foo = 'woohoo!';
%     case 'uparrow'
%         set(S.fh,'pos',P+[0 5 0 0])
%     case 'downarrow'
%         set(S.fh,'pos',P+[0 -5 0 0])
%     case 't'
%         foo = 't!';
%     case 'T'
%         foo = 'T!';
%     case 'm'
%         state.action = 'magnets'; state.face = []; state.direction = [];
%     otherwise  
% end
