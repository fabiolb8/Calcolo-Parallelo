function conds=condFor(NMAX)

conds=zeros(NMAX,1);
for i=1:NMAX
    A=rand(i);
    conds(i)=cond(A);
end
    
end

