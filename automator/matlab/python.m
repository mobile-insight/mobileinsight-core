function [result status] = python(varargin)
%PYTHON Execute Python command and return the result.
%   PYTHON(PYTHONFILE) calls python script specified by the file PYTHONFILE
%   using appropriate python executable.
%
%   PYTHON(PYTHONFILE,ARG1,ARG2,...) passes the arguments ARG1,ARG2,...
%   to the python script file PYTHONFILE, and calls it by using appropriate
%   python executable.
%
%   RESULT=PYTHON(...) outputs the result of attempted python call.  If the
%   exit status of python is not zero, an error will be returned.
%
%   [RESULT,STATUS] = PYTHON(...) outputs the result of the python call, and
%   also saves its exit status into variable STATUS. 
% 
%   If the Python executable is not available, it can be downloaded from:
%
%   See also SYSTEM, JAVA, MEX.

%   Copyright 1990-2007 The MathWorks, Inc.
%   $Revision: 1.1.4.8 $

cmdString = '';

% Add input to arguments to operating system command to be executed.
% (If an argument refers to a file on the MATLAB path, use full file path.)
for i = 1:nargin
    thisArg = varargin{i};
    if isempty(thisArg) || ~ischar(thisArg)
        error('MATLAB:python:InputsMustBeStrings', 'All input arguments must be valid strings.');
    end
    if i==1
        if exist(thisArg, 'file')==2
            % This is a valid file on the MATLAB path
            if isempty(dir(thisArg))
                % Not complete file specification
                % - file is not in current directory
                % - OR filename specified without extension
                % ==> get full file path
                thisArg = which(thisArg);
            end
        else
            % First input argument is Python File - it must be a valid file
            error('MATLAB:python:FileNotFound', 'Unable to find Python file: %s', thisArg);
        end
    end
  
  % Wrap thisArg in double quotes if it contains spaces
  if any(thisArg == ' ')
    thisArg = ['"', thisArg, '"'];
  end
  
  % Add argument to command string
  cmdString = [cmdString, ' ', thisArg];
end

% Execute Python script
errTxtNoPython = 'Unable to find Python executable.';

if isempty(cmdString)
  error('MATLAB:python:NoPythonCommand', 'No python command specified');
elseif ispc
  % PC
  pythonCmd = fullfile(matlabroot, 'sys\python\win32\bin\');
  cmdString = ['python' cmdString];	 
  pythonCmd = ['set PATH=',pythonCmd, ';%PATH%&' cmdString];
  [status, result] = dos(pythonCmd);
else
  % UNIX
  [status ignore] = unix('which python'); %#ok
  if (status == 0)
    cmdString = ['python', cmdString];
    [status, result] = unix(cmdString);
  else
    error('MATLAB:python:NoExecutable', errTxtNoPython);
  end
end

% Check for errors in shell command
if nargout < 2 && status~=0
  error('MATLAB:python:ExecutionError', ...
        'System error: %sCommand executed: %s', result, cmdString);
end

