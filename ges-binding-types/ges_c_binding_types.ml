module Base = struct
  let void = Ctypes.void
  let filename = Ctypes.string
end
            
module M(F : Ctypes.TYPE) = struct
  let void = Base.void
  let filename = Base.filename
end
