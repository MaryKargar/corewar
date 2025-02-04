;redcode-94
;name Warrior-TrainedExt
;assert CORESIZE==8000
spl.ab #9, {1
djn.x  #0, {0
nop.b  $3, }7
jmz.i  3, #6
mod.b  8, *3
mul.ab @3, {6
jmp.a  @7, 1
seq.a  {0, @4
sne.ab 6, *8
sne.f  $3, >7
end