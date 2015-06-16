function varargout = ppr_gui(varargin)
%PPR_GUI M-file for ppr_gui.fig
%      PPR_GUI, by itself, creates a new PPR_GUI or raises the existing
%      singleton*.
%
%      H = PPR_GUI returns the handle to a new PPR_GUI or the handle to
%      the existing singleton*.
%
%      PPR_GUI('Property','Value',...) creates a new PPR_GUI using the
%      given property value pairs. Unrecognized properties are passed via
%      varargin to ppr_gui_OpeningFcn.  This calling syntax produces a
%      warning when there is an existing singleton*.
%
%      PPR_GUI('CALLBACK') and PPR_GUI('CALLBACK',hObject,...) call the
%      local function named CALLBACK in PPR_GUI.M with the given input
%      arguments.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help ppr_gui

% Last Modified by GUIDE v2.5 15-Apr-2013 11:19:25

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @ppr_gui_OpeningFcn, ...
                   'gui_OutputFcn',  @ppr_gui_OutputFcn, ...
                   'gui_LayoutFcn',  [], ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
   gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before ppr_gui is made visible.
function ppr_gui_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   unrecognized PropertyName/PropertyValue pairs from the
%            command line (see VARARGIN)

% Choose default command line output for ppr_gui
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

disable_vicon_Callback(hObject, eventdata, handles);
disable_joystick_Callback(hObject, eventdata, handles);

% UIWAIT makes ppr_gui wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = ppr_gui_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;

% --- Executes on button press in land.
function land_Callback(hObject, eventdata, handles)
% hObject    handle to land (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
'land'

% --- Executes on button press in estop.
function estop_Callback(hObject, eventdata, handles)
% hObject    handle to estop (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
'estop'


function Kp_roll_Callback(hObject, eventdata, handles)
% hObject    handle to Kp_roll (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of Kp_roll as text
%        str2double(get(hObject,'String')) returns contents of Kp_roll as a double



function Kd_roll_Callback(hObject, eventdata, handles)
% hObject    handle to Kd_roll (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of Kd_roll as text
%        str2double(get(hObject,'String')) returns contents of Kd_roll as a double


% --- Executes during object creation, after setting all properties.
function Kd_roll_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Kd_roll (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in takeoff.
function takeoff_Callback(hObject, eventdata, handles)
% hObject    handle to takeoff (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
'takeoff'

% --- Executes on button press in upload_pitch.
function upload_pitch_Callback(hObject, eventdata, handles)
% hObject    handle to upload_pitch (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)



function Kp_pitch_Callback(hObject, eventdata, handles)
% hObject    handle to Kp_pitch (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of Kp_pitch as text
%        str2double(get(hObject,'String')) returns contents of Kp_pitch as a double


% --- Executes during object creation, after setting all properties.
function Kp_pitch_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Kp_pitch (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function Kd_pitch_Callback(hObject, eventdata, handles)
% hObject    handle to Kd_pitch (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of Kd_pitch as text
%        str2double(get(hObject,'String')) returns contents of Kd_pitch as a double


% --- Executes during object creation, after setting all properties.
function Kd_pitch_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Kd_pitch (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in upload_yaw.
function upload_yaw_Callback(hObject, eventdata, handles)
% hObject    handle to upload_yaw (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)



function Kp_yaw_Callback(hObject, eventdata, handles)
% hObject    handle to Kp_yaw (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of Kp_yaw as text
%        str2double(get(hObject,'String')) returns contents of Kp_yaw as a double


% --- Executes during object creation, after setting all properties.
function Kp_yaw_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Kp_yaw (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function Kd_yaw_Callback(hObject, eventdata, handles)
% hObject    handle to Kd_yaw (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of Kd_yaw as text
%        str2double(get(hObject,'String')) returns contents of Kd_yaw as a double


% --- Executes during object creation, after setting all properties.
function Kd_yaw_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Kd_yaw (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in hover.
function hover_Callback(hObject, eventdata, handles)
% hObject    handle to hover (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
'hover'


function hover_duration_Callback(hObject, eventdata, handles)
% hObject    handle to hover_duration (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of hover_duration as text
%        str2double(get(hObject,'String')) returns contents of hover_duration as a double


% --- Executes during object creation, after setting all properties.
function hover_duration_CreateFcn(hObject, eventdata, handles)
% hObject    handle to hover_duration (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in goto_waypoint.
function goto_waypoint_Callback(hObject, eventdata, handles)
% hObject    handle to goto_waypoint (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)



function waypoint_Callback(hObject, eventdata, handles)
% hObject    handle to waypoint (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of waypoint as text
%        str2double(get(hObject,'String')) returns contents of waypoint as a double


% --- Executes during object creation, after setting all properties.
function waypoint_CreateFcn(hObject, eventdata, handles)
% hObject    handle to waypoint (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in move_forward.
function move_forward_Callback(hObject, eventdata, handles)
% hObject    handle to move_forward (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in move_backward.
function move_backward_Callback(hObject, eventdata, handles)
% hObject    handle to move_backward (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in move_left.
function move_left_Callback(hObject, eventdata, handles)
% hObject    handle to move_left (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in move_right.
function move_right_Callback(hObject, eventdata, handles)
% hObject    handle to move_right (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)



function xy_step_Callback(hObject, eventdata, handles)
% hObject    handle to xy_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of xy_step as text
%        str2double(get(hObject,'String')) returns contents of xy_step as a double


% --- Executes during object creation, after setting all properties.
function xy_step_CreateFcn(hObject, eventdata, handles)
% hObject    handle to xy_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in move_up.
function move_up_Callback(hObject, eventdata, handles)
% hObject    handle to move_up (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in move_down.
function move_down_Callback(hObject, eventdata, handles)
% hObject    handle to move_down (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes during object creation, after setting all properties.
function update_x_est_CreateFcn(hObject, eventdata, handles)
% hObject    handle to update_x_est (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called


% --- Executes during object creation, after setting all properties.
function update_y_est_CreateFcn(hObject, eventdata, handles)
% hObject    handle to update_y_est (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called


% --- Executes during object creation, after setting all properties.
function update_z_est_CreateFcn(hObject, eventdata, handles)
% hObject    handle to update_z_est (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called


% --- Executes during object creation, after setting all properties.
function edit16_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit16 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called



function z_step_Callback(hObject, eventdata, handles)
% hObject    handle to z_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of z_step as text
%        str2double(get(hObject,'String')) returns contents of z_step as a double


% --- Executes during object creation, after setting all properties.
function z_step_CreateFcn(hObject, eventdata, handles)
% hObject    handle to z_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in enable_joystick.
function enable_joystick_Callback(hObject, eventdata, handles)
% hObject    handle to enable_joystick (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
set(handles.joystick_status,'string','Joystick Connected','enable','on','BackgroundColor','green');

% --- Executes on button press in disable_joystick.
function disable_joystick_Callback(hObject, eventdata, handles)
% hObject    handle to disable_joystick (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
'done!'
set(handles.joystick_status,'string','Joystick Disonnected','enable','on','BackgroundColor','red');

% --- Executes on button press in joystick_status.
function joystick_status_Callback(hObject, eventdata, handles)
% hObject    handle to joystick_status (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in enable_vicon.
function enable_vicon_Callback(hObject, eventdata, handles)
% hObject    handle to enable_vicon (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
set(handles.vicon_status,'string','Vicon Connected','enable','on','BackgroundColor','green');


% --- Executes on button press in disable_vicon.
function disable_vicon_Callback(hObject, eventdata, handles)
% hObject    handle to disable_vicon (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
set(handles.vicon_status,'string','Vicon Disonnected','enable','on','BackgroundColor','red');

% --- Executes on button press in vicon_status.
function vicon_status_Callback(hObject, eventdata, handles)
% hObject    handle to vicon_status (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
