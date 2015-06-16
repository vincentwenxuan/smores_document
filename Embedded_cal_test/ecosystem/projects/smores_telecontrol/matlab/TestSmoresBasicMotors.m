classdef TestSmoresBasicMotors < matlab.unittest.TestCase
    %TestSmoresTeleop: Set of basic tests for first SMORES module.
    %
    
    properties
        S;              % Smores_Teleop object
        real_module;    % true if we are testing a real module, false if exploded setup.
        spin_time;
    end
    
    methods (TestClassSetup)
        function setUpSmoresTeleop(obj)
            obj.real_module = true;
            obj.S = Smores_Teleop;
            obj.spin_time = 0.5;
        end
    end
    
    methods (Test)
        function testLeftWheel(obj)
            disp('Left');
            repeat = true;
            while(repeat)
                obj.S.moveDofTime('left', 'ccw', 50, obj.spin_time);
                pause(0.5);
                obj.S.moveDofTime('left', 'cw', 50, obj.spin_time);
                if obj.real_module
                    userAnswer = input('Did left face spin ccw, then cw? [y]/n:  ','s');
                else
                    userAnswer = input('Did motor [?] spin cw, then ccw? [y]/n: ','s');
                end
                if(~strcmp(userAnswer,'r'))
                    repeat = false;
                end
            end
            obj.verifyTrue(obj.isYes(userAnswer));
        end
        function testRightWheel(obj)
            disp('Right')
            repeat = true;
            while(repeat)
                obj.S.moveDofTime('right', 'ccw', 50, obj.spin_time);
                pause(0.5);
                obj.S.moveDofTime('right', 'cw', 50, obj.spin_time);
                if obj.real_module
                    userAnswer = input('Did right face spin ccw, then cw? [y]/n/r:  ','s');
                else
                    userAnswer = input('Did motor [?] spin cw, then ccw? [y]/n/r: ','s');
                end
                if(~ strcmp(userAnswer,'r'))
                    repeat = false;
                end
            end
            obj.verifyTrue(obj.isYes(userAnswer));
        end
        function testPan(obj)
            disp('Top pan');
            repeat = true;
            while(repeat)
                obj.S.moveDofTime('pan', 'ccw', 50, obj.spin_time);
                pause(0.5);
                obj.S.moveDofTime('pan', 'cw', 50, obj.spin_time);
                if obj.real_module
                    userAnswer = input('Did top face spin ccw, then cw? [y]/n:  ','s');
                else
                    userAnswer = input('Did motor [?] spin cw, then ccw?, while motor [?] spun ccw, then cw? [y]/n: ','s');
                end
                if(~strcmp(userAnswer,'r'))
                    repeat = false;
                end
            end
            obj.verifyTrue(obj.isYes(userAnswer));
        end
        function testTilt(obj)
            disp('Middle Tilt');
            repeat = true;
            while(repeat)
                obj.S.moveDofTime('tilt', 'ccw', 50, obj.spin_time);
                pause(0.5);
                obj.S.moveDofTime('tilt', 'cw', 50, obj.spin_time);
                if obj.real_module
                    userAnswer = input('Did bend joint spin ccw, then cw? [y]/n:  ','s');
                else
                    userAnswer = input('Did motor [?] spin cw, then ccw?, while motor [?] spun ccw, then cw? [y]/n: ','s');
                end
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

