function state = getViconState(viconObject, viconVehicle);
% Get a frame
% while comms.viconClient.GetFrame().Result.Value ~= Result.Success
% end% while
if viconObject.GetFrame().Result.Value ~= Result.Success
    error('Vicon returned failed');
end

SubjectCount = viconObject.GetSubjectCount().SubjectCount;

% Find the right vicon subject
for SubjectIndex = 1:SubjectCount
    SubjectName = viconObject.GetSubjectName( SubjectIndex ).SubjectName;
    if strcmp(SubjectName,viconVehicle)
        %Get data from vicon
        translationXYZ = viconObject.GetSegmentGlobalTranslation( SubjectName, SubjectName ).Translation; %subjectName, SegmentName
        orientationXYZ = viconObject.GetSegmentGlobalRotationMatrix( SubjectName, SubjectName ).Rotation;     
        
        state.R = reshape(orientationXYZ,3,3).';
        state.x = translationXYZ(1:3)/1000; %mm > m
    end
end