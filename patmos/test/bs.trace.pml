---
format: pml-0.1
triple: patmos-unknown-unknown-elf
machine-functions:
- name: 3
  level: machinecode
  mapsto: main
  hash: 0
  blocks:
  - name: 0
    mapsto: entry
    predecessors: []
    successors:
    - 1
    instructions:
    - index: 0
      opcode: LIi
      size: 4
      address: 132100
    - index: 1
      opcode: MOV
      size: 4
      bundled: true
      address: 132104
    - index: 2
      opcode: MFS
      size: 4
      address: 132108
    - index: 3
      opcode: LIi
      size: 4
      bundled: true
      address: 132112
    address: 132100
  - name: 1
    mapsto: while.body.i
    predecessors:
    - 0
    - 2
    successors:
    - 3
    - 2
    loops:
    - 1
    instructions:
    - index: 0
      opcode: ADDr
      size: 4
      address: 132116
    - index: 1
      opcode: SRAi
      size: 4
      address: 132120
    - index: 2
      opcode: CMPIEQ
      size: 4
      address: 132124
    - index: 3
      opcode: BR
      size: 4
      branch-type: conditional
      branch-delay-slots: 2
      branch-targets:
      - 3
      address: 132128
    - index: 4
      opcode: NOP
      size: 4
      address: 132132
    - index: 5
      opcode: NOP
      size: 4
      address: 132136
    address: 132116
  - name: 2
    mapsto: if.else.i
    predecessors:
    - 1
    successors:
    - 4
    - 1
    loops:
    - 1
    instructions:
    - index: 0
      opcode: SUBi
      size: 4
      address: 132140
    - index: 1
      opcode: CMPIULT
      size: 4
      address: 132144
    - index: 2
      opcode: ADDl_ow
      size: 8
      address: 132148
    - index: 3
      opcode: ADDi_ow
      size: 4
      address: 132156
    - index: 4
      opcode: CMPLT
      size: 4
      address: 132160
    - index: 5
      opcode: BR
      size: 4
      branch-type: conditional
      branch-delay-slots: 2
      branch-targets:
      - 1
      address: 132164
    - index: 6
      opcode: NOP
      size: 4
      address: 132168
    - index: 7
      opcode: LIi
      size: 4
      address: 132172
    - index: 8
      opcode: SUBi
      size: 4
      bundled: true
      address: 132176
    - index: 9
      opcode: BRu
      size: 4
      branch-type: unconditional
      branch-delay-slots: 2
      branch-targets:
      - 4
      address: 132180
    - index: 10
      opcode: NOP
      size: 4
      address: 132184
    - index: 11
      opcode: NOP
      size: 4
      address: 132188
    address: 132140
  - name: 3
    mapsto: if.end
    predecessors:
    - 1
    successors:
    - 4
    instructions:
    - index: 0
      opcode: MOVrp
      size: 4
      address: 132192
    - index: 1
      opcode: MOVpr
      size: 4
      address: 132196
    address: 132192
  - name: 4
    mapsto: return
    predecessors:
    - 2
    - 3
    successors: []
    instructions:
    - index: 0
      opcode: RET
      size: 4
      branch-type: return
      branch-delay-slots: 3
      address: 132200
    - index: 1
      opcode: NOP
      size: 4
      address: 132204
    - index: 2
      opcode: NOP
      size: 4
      address: 132208
    - index: 3
      opcode: MTS
      size: 4
      address: 132212
    address: 132200
  subfunctions:
  - name: 0
    blocks:
    - 0
    - 1
    - 2
    - 3
    - 4
bitcode-functions:
- name: main
  level: bitcode
  hash: 0
  blocks:
  - name: entry
    predecessors: []
    successors:
    - while.body.i
    instructions:
    - index: 0
      callees:
      - llvm.dbg.value
    - index: 1
      callees:
      - llvm.dbg.value
    - index: 2
      callees:
      - llvm.dbg.value
    - index: 3
      callees:
      - llvm.dbg.value
    - index: 4
  - name: while.body.i
    predecessors:
    - if.else.i
    - entry
    successors:
    - if.end
    - if.else.i
    loops:
    - while.body.i
    instructions:
    - index: 0
      opcode: lsr.iv
    - index: 1
      opcode: low.06.i
    - index: 2
      opcode: up.05.i
    - index: 3
    - index: 4
    - index: 5
      callees:
      - llvm.dbg.value
    - index: 6
    - index: 7
  - name: if.else.i
    predecessors:
    - while.body.i
    successors:
    - return
    - while.body.i
    loops:
    - while.body.i
    instructions:
    - index: 0
    - index: 1
    - index: 2
    - index: 3
      callees:
      - llvm.dbg.value
    - index: 4
    - index: 5
      callees:
      - llvm.dbg.value
    - index: 6
      opcode: up.1.i
    - index: 7
      opcode: low.1.i
    - index: 8
    - index: 9
      opcode: lsr.iv.next
    - index: 10
  - name: if.end
    predecessors:
    - while.body.i
    successors:
    - return
    instructions:
    - index: 0
      opcode: not.
    - index: 1
      opcode: .
    - index: 2
  - name: return
    predecessors:
    - if.else.i
    - if.end
    successors: []
    instructions:
    - index: 0
      opcode: 0.0
    - index: 1
relation-graphs:
- src:
    function: main
    level: bitcode
  dst:
    function: 3
    level: machinecode
  nodes:
  - name: 0
    type: entry
    src-block: entry
    dst-block: 0
    src-successors:
    - 2
    dst-successors:
    - 2
  - name: 1
    type: exit
  - name: 2
    type: progress
    src-block: while.body.i
    dst-block: 1
    src-successors:
    - 3
    - 4
    dst-successors:
    - 3
    - 4
  - name: 3
    type: progress
    src-block: if.else.i
    dst-block: 2
    src-successors:
    - 5
    - 2
    dst-successors:
    - 5
    - 2
  - name: 4
    type: progress
    src-block: if.end
    dst-block: 3
    src-successors:
    - 5
    dst-successors:
    - 5
  - name: 5
    type: progress
    src-block: return
    dst-block: 4
    src-successors:
    - 1
    dst-successors:
    - 1
  status: valid
flowfacts:
- scope:
    function: 3
    loop: 1
  lhs:
  - factor: 1
    program-point:
      function: 3
      block: 1
  op: less-equal
  rhs: '4'
  level: machinecode
  origin: trace
- scope:
    function: 3
  lhs:
  - factor: 1
    program-point:
      function: 3
      block: 0
  op: less-equal
  rhs: '1'
  level: machinecode
  origin: trace
- scope:
    function: 3
  lhs:
  - factor: 1
    program-point:
      function: 3
      block: 1
  op: less-equal
  rhs: '4'
  level: machinecode
  origin: trace
- scope:
    function: 3
  lhs:
  - factor: 1
    program-point:
      function: 3
      block: 2
  op: less-equal
  rhs: '3'
  level: machinecode
  origin: trace
- scope:
    function: 3
  lhs:
  - factor: 1
    program-point:
      function: 3
      block: 3
  op: less-equal
  rhs: '1'
  level: machinecode
  origin: trace
- scope:
    function: 3
  lhs:
  - factor: 1
    program-point:
      function: 3
      block: 4
  op: less-equal
  rhs: '1'
  level: machinecode
  origin: trace
timing:
- scope:
    function: 3
  cycles: 56
  level: machinecode
  origin: trace
- scope:
    function: 3
  cycles: 65
  level: machinecode
  origin: platin
  cache-cycles: 0
  profile:
  - reference:
      function: 3
      edgesource: 0
      edgetarget: 1
    cycles: 2
    wcet-frequency: 1
    wcet-contribution: 2
  - reference:
      function: 3
      edgesource: 1
      edgetarget: 3
    cycles: 6
    wcet-frequency: 1
    wcet-contribution: 6
  - reference:
      function: 3
      edgesource: 1
      edgetarget: 2
    cycles: 6
    wcet-frequency: 3
    wcet-contribution: 18
  - reference:
      function: 3
      edgesource: 2
      edgetarget: 1
    cycles: 11
    wcet-frequency: 3
    wcet-contribution: 33
  - reference:
      function: 3
      edgesource: 3
      edgetarget: 4
    cycles: 2
    wcet-frequency: 1
    wcet-contribution: 2
  - reference:
      function: 3
      edgesource: 4
    cycles: 4
    wcet-frequency: 1
    wcet-contribution: 4
