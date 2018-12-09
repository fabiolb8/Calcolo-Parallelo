function julia_cpu(x,c,n,iterazioni,~)
%x=1.5, c=-0.75, n=400, iterazioni=20
% insieme Julia set
f = @(x) x.^2+c;
xval=linspace(-x,x,n);
yval=xval;
[X,Y]=meshgrid(xval,yval);
Z=X+1i*Y;
for k=1:iterazioni
	Z=f(Z);
end
W = exp(-abs(Z));

if nargin==5
    imagesc(W);
    colormap(prism(256))
    shading flat;
    axis('square','equal','off');
end

end