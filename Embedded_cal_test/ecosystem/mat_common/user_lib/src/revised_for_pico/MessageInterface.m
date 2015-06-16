%{
    MessageIface class, provides the same interface as the 
    mbed implementation ... but in matlab 
%}

classdef MessageInterface < handle
   % The following properties can be set only by class methods
   properties (SetAccess = private)
      serial_handle
      input_buffer
   end
   
   properties (SetAccess = private, GetAccess = private)
       serial_jobject
   end
   
   methods
       % Instantiation
       function self = MessageInterface( port_name, baudrate )
           [ self.serial_handle ] = initializeSerial( port_name,  baudrate);
           self.input_buffer = uint8([]);
           self.serial_jobject = igetfield(self.serial_handle, 'jobject');
           try
                uselessVar = toc;
            catch
                tic;
            end % try catch
       end
       
       function delete( self )
           % clean up the serial object
           fclose(self.serial_handle);
           delete(self.serial_handle);
       end

       function Flush( self )
           while true
               pkt_data = mexPeekPacket( uint8([]) );
               if isempty(pkt_data)
                   break;
               end
           end
           while true
               bytes_available = self.serial_jobject.BytesAvailable;
%                bytes_available = get(self.serial_handle, 'BytesAvailable');
               if ~bytes_available
                    self.input_buffer = uint8([]);
                    break;
               end
               fread(self.serial_jobject, bytes_available, 5, 0);
           end
       end
       
       % getBytes from serial
       function GetBytes( self )
           bytes_available = self.serial_jobject.BytesAvailable;
%             bytes_available = get(self.serial_jobject, 'BytesAvailable');
%            bytes_available = get(self.serial_handle, 'BytesAvailable');
           if ~bytes_available
               return
           end
%            self.input_buffer = [self.input_buffer; fread(self.serial_handle, bytes_available)];
            out = fread(self.serial_jobject, bytes_available, 5, 0);
            self.input_buffer = [self.input_buffer; typecastx(out(1),'uint8')];
       end
       
       % peekPacket to see if there is an available packet
       function [msg_type, pkt] = PeekPacket( self ) 
           msg_type = [];
           pkt = [];
           % Chopping buffer into 100 byte segments if too large
           buf = self.input_buffer;
           if ~isempty(buf)
               if length(buf) <= 100
                   buffer = buf;
                   self.input_buffer = uint8([]);
               else % length(buf) > 100
                   buffer = buf(1:100);
                   self.input_buffer = buf(100:end);
               end
           else % buffer is empty
               buffer = uint8([]);
           end
           
           pkt_data = mexPeekPacket( buffer );
           if isempty(pkt_data)
               return;
           end
           msg_type = pkt_data(1);
           pkt = pkt_data(2:end);

           
%            % Chopping buffer into 100 byte segments if too large
%            buf = self.input_buffer;
%            if length(buf) > 100
%                buffer = buf(1:100);
%                self.input_buffer = buf(100:end);
%            else
%                buffer = buf;
%                self.input_buffer = uint8([]);
%            end
%            
%            pkt_data = mexPeekPacket( buffer );
%            if isempty(pkt_data)
%                msg_type = [];
%                pkt = [];
%                return;
%            end
%            msg_type = pkt_data(1);
%            pkt = pkt_data(2:end);
       end
       
       % sendPacket 
       function SendPacket( self, pkt )
           try
               fwrite(self.serial_jobject, pkt, length(pkt), 5, 0, 0); % slightly less safe, but is what fwrite(self.serial_handle,pkt) sends eventually
           catch ex
           end
       end
       
       % sendMsg, wrapper around pack_msg 
       function SendMsg( self, msg_spec, msg ) 
           pkt = PackMsg( msg_spec, msg );
           self.SendPacket( pkt );
       end
       
       % unpackMsg, wrapper around unpack_msg
       function msg = UnpackMsg( self, msg_spec, pkt_data )
           msg = UnpackMsg( msg_spec, pkt_data );
       end
   end
end
   
   
   

