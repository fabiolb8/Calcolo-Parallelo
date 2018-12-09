function y = prodMatriciPar(A,B,dim)

y=zeros(4,4);
    
    parfor j=1:dim
        for k=1:dim
             for z=1:dim
                y = y(j,k) + A(j,z).*B(z,j);
            end
        end
    end
    
end

