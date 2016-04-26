function [ out ] = get_note(freq)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
table = ...
[261.63%C
277.18%Db
293.66%D
311.13%Eb
329.63%E
349.23%F
369.99%F#
392.00%G
415.30%Ab
440.00%A
466.16%Bb
493.88%B
523.25%C  1
554.37%Db 2
587.33%D  3
622.25%   4
659.25%   5
698.46%   6
739.99%   7
783.99%   8
830.61%   9
880.00%   10
932.33%   11
987.77%   12
1046.50%  13
];
map = cellstr(['C4_'; 'Db4'; 'D4_'; 'Eb4'; 'E4_'; 'F4_'; 'F#4'; 'G4_'; 'Ab4'; 'A4_'; 'Bb4'; 'B4_'; 'C5_'; ...
    'Db5'; 'D5_'; 'Eb5'; 'E5_'; 'F5_'; 'F#5'; 'G5_'; 'Ab5'; 'A5_'; 'Bb5'; 'B5_'; 'C6_']);
[c idx] = min(abs(table-freq));
out = char(map(idx));

end

