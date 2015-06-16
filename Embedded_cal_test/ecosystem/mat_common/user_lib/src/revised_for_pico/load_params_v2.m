function [gains, stick_gains] = load_params(quadname)

switch quadname
    case 'herald'
        gains.kpxy = 30; % slow = 100, electric = 30
        gains.kdxy = 30; % slow = 30, electric = 30
        gains.kpz = 100;
        gains.kdz = 30;
        stick_gains = [1 .35 1 2 2];
        
    case 'passive_active' % Tune these
        gains.kpxy = 22;
        gains.kdxy = 18;
        gains.kpz = 300;
        gains.kdz = 150;
        stick_gains = [1 1 1 3 3];
        
    case 'foldedquad' % Tune these
        gains.kpxy = 700;
        gains.kdxy = 100;
        gains.kpz = 300;
        gains.kdz = 150;
        stick_gains = [1 1 1 1 1];
        
    case 'passive' % Tune these
        gains.kpxy = 700;
        gains.kdxy = 20;
        gains.kpz = 300;
        gains.kdz = 150;
        stick_gains = [1 1 1 3 3];
        
    case 'carbonfiber' % Tuned
        gains.kpxy = 600;
        gains.kdxy = 150;
        gains.kpz = 300;
        gains.kdz = 150;
        stick_gains = [3 3 2 1 1];
        
    case 'picoquad' % Tune these
        gains.kpxy = 500;
        gains.kdxy = 130;
        gains.kpz = 300;
        gains.kdz = 150;
        stick_gains = [3 3 2 1 1];
        
    case 'allpcb' % Tune these
        gains.kpxy = 500;
        gains.kdxy = 90;
        gains.kpz = 300;
        gains.kdz = 150;
        stick_gains = [3 3 2 1 1];
        
    case 'allpcb_up' % Tune these
        gains.kpxy = 500;
        gains.kdxy = 150;%90;
        gains.kpz = 300;
        gains.kdz = 150;
        stick_gains = [3 3 2 1 1];
        
    case 'planar' % Tune these
        gains.kpxy = 350;
        gains.kdxy = 80;
        gains.kpz = 300;
        gains.kdz = 100;
        stick_gains = [3 3 1 1 1]; %[3 3 1 1 1];
        
    case 'planar_ekf' % Tune these
        gains.kpxy = 2800;
        gains.kdxy = 660;
        gains.kpz = 2800;
        gains.kdz = 660;
        stick_gains = [3 3 1 1 1]; %[3 3 1 1 1];
        %[R P T ? Y]
        
    case 'zero_gains' % Tune these
        gains.kpxy = 0.00141;
        gains.kdxy = 0.000283;
        gains.kpz = 0.00718;%0.00581;
        gains.kdz = 0.000718;%0.000581;
        stick_gains = [3 3 1 1 1]; %[3 3 1 1 1];
        %[R P T ? Y]
    otherwise
        warning('INVALID NAME !!!')
end

end