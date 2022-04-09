| test | dom0 | dom0-tcp | dom0-stdio | dom0-unixshm |
| :--- | :-: | :-: | :-: | :-: |
| [build] use-vbo=false | 8463 | 2762 (/3.06408 -67%) | 2012 (/4.20626 -76%) | 4287 (/1.97411 -49%) | 
| [build] use-vbo=true | 10964 | 4840 (/2.26529 -55%) | 5560 (/1.97194 -49%) | 7710 (/1.42205 -29%) | 
| [texture] texture-filter=nearest | 9509 | 4152 (/2.29022 -56%) | 4814 (/1.97528 -49%) | 7179 (/1.32456 -24%) | 
| [texture] texture-filter=linear | 9441 | 4141 (/2.27988 -56%) | 4819 (/1.95912 -48%) | 7183 (/1.31435 -23%) | 
| [texture] texture-filter=mipmap | 9190 | 4129 (/2.22572 -55%) | 4782 (/1.92179 -47%) | 7113 (/1.292 -22%) | 
| [shading] shading=gouraud | 8909 | 4669 (/1.90812 -47%) | 5319 (/1.67494 -40%) | 7196 (/1.23805 -19%) | 
| [shading] shading=blinn-phong-inf | 8564 | 4623 (/1.85248 -46%) | 5239 (/1.63466 -38%) | 7228 (/1.18484 -15%) | 
| [shading] shading=phong | 8507 | 4497 (/1.89171 -47%) | 5197 (/1.63691 -38%) | 7208 (/1.18022 -15%) | 
| [shading] shading=cel | 8209 | 4493 (/1.82706 -45%) | 5161 (/1.59058 -37%) | 7087 (/1.15832 -13%) | 
| [bump] bump-render=high-poly | 5650 | 4281 (/1.31979 -24%) | 4831 (/1.16953 -14%) | 5476 (/1.03178 -3%) | 
| [bump] bump-render=normals | 11010 | 4595 (/2.39608 -58%) | 5264 (/2.09157 -52%) | 7593 (/1.45002 -31%) | 
| [bump] bump-render=height | 10848 | 3907 (/2.77655 -63%) | 4611 (/2.35264 -57%) | 7377 (/1.47052 -31%) | 
| [effect2d] kernel=0,1,0;1,-4,1;0,1,0; | 7348 | 5160 (/1.42403 -29%) | 5838 (/1.25865 -20%) | 6875 (/1.0688 -6%) | 
| [effect2d] kernel=1,1,1,1,1;1,1,1,1,1;1,1,1,1,1; | 3771 | 3418 (/1.10328 -9%) | 3597 (/1.04837 -4%) | 3741 (/1.00802 +0%) | 
| [pulsar] light=false:quads=5:texture=false | 8329 | 2439 (/3.41492 -70%) | 2953 (/2.82052 -64%) | 5757 (/1.44676 -30%) | 
| [desktop] blur-radius=5:effect=blur:passes=1:separable=true:windows=4 | 3593 | 621 (/5.78583 -82%) | 755 (/4.75894 -78%) | 2439 (/1.47314 -32%) | 
| [desktop] effect=shadow:windows=4 | 6988 | 356 (/19.6292 -94%) | 433 (/16.1386 -93%) | 1604 (/4.35661 -77%) | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=map | 1317 | - | - | - | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=subdata | 1801 | 770 (/2.33896 -57%) | 745 (/2.41745 -58%) | 1348 (/1.33605 -25%) | 
| [buffer] columns=200:interleave=true:update-dispersion=0.9:update-fraction=0.5:update-method=map | 1794 | - | - | - | 
| [ideas] speed=duration | 5224 | 318 (/16.4277 -93%) | 389 (/13.4293 -92%) | 1418 (/3.68406 -72%) | 
| [jellyfish] &lt;default&gt; | 5847 | 2474 (/2.36338 -57%) | 2943 (/1.98675 -49%) | 5204 (/1.12356 -10%) | 
| [terrain] &lt;default&gt; | 478 | 473 (/1.01057 -1%) | 474 (/1.00844 +0%) | 480 (/0.995833 +0%) | 
| [shadow] &lt;default&gt; | 6353 | 2603 (/2.44065 -59%) | 3060 (/2.07614 -51%) | 5585 (/1.13751 -12%) | 
| [refract] &lt;default&gt; | 804 | 793 (/1.01387 -1%) | 815 (/0.986503 +1%) | 812 (/0.990148 +0%) | 
| [conditionals] fragment-steps=0:vertex-steps=0 | 9076 | 5253 (/1.72777 -42%) | 6028 (/1.50564 -33%) | 7560 (/1.20053 -16%) | 
| [conditionals] fragment-steps=5:vertex-steps=0 | 8807 | 5198 (/1.69431 -40%) | 5960 (/1.47768 -32%) | 7429 (/1.18549 -15%) | 
| [conditionals] fragment-steps=0:vertex-steps=5 | 9092 | 5245 (/1.73346 -42%) | 5922 (/1.53529 -34%) | 7479 (/1.21567 -17%) | 
| [function] fragment-complexity=low:fragment-steps=5 | 8795 | 5179 (/1.6982 -41%) | 5844 (/1.50496 -33%) | 7415 (/1.18611 -15%) | 
| [function] fragment-complexity=medium:fragment-steps=5 | 8781 | 5186 (/1.69321 -40%) | 5919 (/1.48353 -32%) | 7355 (/1.19388 -16%) | 
| [loop] fragment-loop=false:fragment-steps=5:vertex-steps=5 | 8793 | 5240 (/1.67805 -40%) | 5928 (/1.4833 -32%) | 7440 (/1.18185 -15%) | 
| [loop] fragment-steps=5:fragment-uniform=false:vertex-steps=5 | 8808 | 5187 (/1.69809 -41%) | 5934 (/1.48433 -32%) | 7424 (/1.18642 -15%) | 
| [loop] fragment-steps=5:fragment-uniform=true:vertex-steps=5 | 8785 | 5203 (/1.68845 -40%) | 5928 (/1.48195 -32%) | 7427 (/1.18285 -15%) | 
| glmark2 Score | 7086 | 3619 (/1.958 -48%) | 4099 (/1.72871 -42%) | 5723 (/1.23816 -19%) | 
