function [ out ] = inPlaceFFT( X, Wnk, L )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

stages = log2(L);
groups = 1;
tieFighters = L/2;
temp = 0;

for stage = 1:stages
    for group = 1:groups
        for tieFighter = 1:tieFighters
            jumpBy = (group-1)*tieFighters*2;
            position1 = tieFighter+jumpBy;
            l_i = tieFighters+position1;
            X(l_i) = X(l_i)*Wnk(group);
            temp = X(position1)+X(l_i);
            X(l_i) = X(position1)-X(l_i);
            X(position1) = temp;
        end
    end
    groups = groups * 2;
    tieFighters = tieFighters/2;
end

out = zeros(1,L);

for n = 0:L-1
    out(bin2dec(fliplr(dec2bin(n,stages)))+1) = X(n+1);
end