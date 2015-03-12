# Introduction #

OpenGL lights seem more advanced then GX. So how should these be implemented in gx. If possible at all.

# Theory #

xg has only one light color (a diffuse one)
opengl lights have the following colors: diffuse, ambient and specular
also opengl has a global ambient color independend of a individual light

gx has 2 material colors: diffuse (material) and ambient (can also be considered part of light?)
opengl material have the followning colors: diffuse, ambient, specular and emission

so we (may) have problem (or call it a challenge)

now how does opengl calculate light with all these colors
```
vertex color = material emission 
             + global ambient scaled by material ambient
             + ambient, diffuse, specular contributions from light(s), properly attinuated
```

let us take these apart.

  * material emission is like a constant color. So we can just add this in a tev stage. The only problem is how to add a color to a specific stage?)

  * global ambient scaled by material ambient
this is global ambient `*` material ambient so we can feed that result to an tev stage.

  * Now comes the hard part as each color is used in the light calulation. And we only have once color in gx.
Maybe we are lucky as each colors term is added to each other and only then used in light calculation
So we might get away with just adding the 3 colors upfront and feed that as color to the light source
But first let see how these terms are calculated.

  * Ambient Term = light ambient `*` material ambient
for xg would go in GXChanAmbColor ?

  * Diffuse Term = surface `*` (light diffuse `*` material diffuse)
> for gx this would be:	light diffues = light color and	material diffuse = GXChanMatColor

  * Specular Term = normal.shininess `*` (light specular `*` material specular)

now we could use 3 light to emulate 1 opengl light but that would not be helpfull
so maybe there is an other way also gx material misses color components

gx has at max 2 channels and we need both
each channel can be setup differently so we can have one chanel for normal diffuse and ambient
and the other for specular. But we only have on light color so unless the specular color equals light color this it not usefull)
maybe some experiments with GXChanMatColor help with that? So light color to none and all color CHANMatColor? But that would break diffuse light?
> if instead of using difflightcolor as light color and GXChanMatColor with diffmatcolor can i feed 1,1,1,1 as light color and do diffmatcolor\*difflightcolor in GXSetChanMatColor?

also we have multiple tev stages.
as we have used 2 channels we have to use 3 stages
  * stage 1 = emissive + global ambient scaled by material as constant color (maybe 2 stages?)
  * stage 2 = ambient + diffuse
  * stage 3 = specular

So this might do the trick in theory. Now on to practice...

# Practice #
Once i have something working it will be posted here.

# Feeback #
I you have comments and/or ideas do not hesitate to post them below here.