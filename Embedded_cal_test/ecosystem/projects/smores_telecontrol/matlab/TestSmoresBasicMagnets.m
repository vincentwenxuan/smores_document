classdef TestSmoresBasicMagnets < matlab.unittest.TestCase
    %TestSmoresTeleop: Set of basic tests for first SMORES module.
    %
    
    properties
        S;              % Smores_Teleop object
        real_module;    % true if we are testing a real module, false if exploded setup.
    end
    
    methods (TestClassSetup)
        function setUpSmoresTeleop(obj)
            obj.real_module = false;
            obj.S = Smores_Teleop;
        end
    end
    
    methods (Test)
        %%%%%%%%   Ping Tests %%%%%%%%%
        % Verify that we can at least talk to each face
        function pingLeftFace(obj)
            response = obj.S.pingFace('left');
            obj.verifyEqual(double(response), hex2dec('A'));
        end
        function pingRightFace(obj)
            response = obj.S.pingFace('right');
            obj.verifyEqual(double(response), hex2dec('C'));
        end
        function pingTopFace(obj)
            response = obj.S.pingFace('top');
            obj.verifyEqual(double(response), hex2dec('D'));
        end
        function pingBottomFace(obj)
            response = obj.S.pingFace('bottom');
            obj.verifyEqual(double(response), hex2dec('B'));
        end
        %%%%%%%%    Magnet Tests   %%%%%%%%
        % left
        function testLeftMagnetOn(obj)
            response = obj.S.pingFace('left');
            obj.assumeEqual(double(response), hex2dec('A'));
            disp('Left Magnets');
            repeat = true;
            while(repeat)
                obj.S.magnetControl('left', 'on')
                userAnswer = input('Are all left face magnets on? [y]/n/r: ', 's');
                if(~strcmp(userAnswer,'r'))
                    repeat = false;
                end
            end
            obj.verifyTrue(obj.isYes(userAnswer));
        end
        function testLeftMagnetOff(obj)
            response = obj.S.pingFace('left');
            obj.assumeEqual(double(response), hex2dec('A'));
            repeat = true;
            while(repeat)
                obj.S.magnetControl('left', 'off')
                userAnswer = input('Are all left face magnets off? [y]/n/r: ', 's');
                if(~strcmp(userAnswer,'r'))
                    repeat = false;
                end
            end
            obj.verifyTrue(obj.isYes(userAnswer));
        end
        % right
        function testRightMagnetOn(obj)
            response = obj.S.pingFace('right');
            obj.assumeEqual(double(response), hex2dec('C'));
            disp('Right Magnets');
            repeat = true;
            while(repeat)
                obj.S.magnetControl('right', 'on')
                userAnswer = input('Are all right face magnets on? [y]/n/r: ', 's');
                if(~strcmp(userAnswer,'r'))
                    repeat = false;
                end
            end
            obj.verifyTrue(obj.isYes(userAnswer));
        end
        function testRightMagnetOff(obj)
            response = obj.S.pingFace('right');
            obj.assumeEqual(double(response), hex2dec('C'));
            repeat = true;
            while(repeat)
                obj.S.magnetControl('right', 'off')
                userAnswer = input('Are all right face magnets off? [y]/n/r: ', 's');
                if(~strcmp(userAnswer,'r'))
                    repeat = false;
                end
            end
            obj.verifyTrue(obj.isYes(userAnswer));
        end
        % top
        function testTopMagnetOn(obj)
            response = obj.S.pingFace('top');
            obj.assumeEqual(double(response), hex2dec('D'));
            disp('top Magnets');
            repeat = true;
            while(repeat)
                obj.S.magnetControl('top', 'on')
                userAnswer = input('Are all top face magnets on? [y]/n/r: ', 's');
                if(~strcmp(userAnswer,'r'))
                    repeat = false;
                end
            end
            obj.verifyTrue(obj.isYes(userAnswer));
        end
        function testTopMagnetOff(obj)
            response = obj.S.pingFace('top');
            obj.assumeEqual(double(response), hex2dec('D'));
            repeat = true;
            while(repeat)
                obj.S.magnetControl('top', 'off')
                userAnswer = input('Are all top face magnets off? [y]/n/r: ', 's');
                if(~strcmp(userAnswer,'r'))
                    repeat = false;
                end
            end
            obj.verifyTrue(obj.isYes(userAnswer));
        end
        %bottom 
        function testBottomMagnetOn(obj)
            response = obj.S.pingFace('bottom');
            obj.assumeEqual(double(response), hex2dec('B'));
            disp('bottom Magnets');
            repeat = true;
            while(repeat)
                obj.S.magnetControl('bottom', 'on')
                userAnswer = input('Are all bottom face magnets on? [y]/n/r: ', 's');
                if(~strcmp(userAnswer,'r'))
                    repeat = false;
                end
            end
            obj.verifyTrue(obj.isYes(userAnswer));
        end
        function testBottomMagnetOff(obj)
            response = obj.S.pingFace('bottom');
            obj.assumeEqual(double(response), hex2dec('B'));
            repeat = true;
            while(repeat)
                obj.S.magnetControl('bottom', 'off')
                userAnswer = input('Are all bottom face magnets off? [y]/n/r: ', 's');
                if(~strcmp(userAnswer,'r'))
                    repeat = false;
                end
            end
            obj.verifyTrue(obj.isYes(userAnswer));
        end
    end
    methods (Static = true)
        function [result] = isYes(userAnswer)
            if(strcmp(userAnswer,'') || strcmp(userAnswer,'y'))
                result = true;
            else
                result = false;
            end
        end
    end
end

