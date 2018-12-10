function conds=condParFor(NMAX)

conds=zeros(1,NMAX);
parfor i=1:NMAX
    A=rand(i);
    conds(i)=cond(A);
end
    
end
