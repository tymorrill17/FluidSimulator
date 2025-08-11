# Fluid Simulation Project

## Goals
I want to learn about and play with particle-based fluid simulation techniques to make neat-looking and interactive animations. I will start in 2D then work my way up to 3D. Here are a few goals I have in mind for this project:
1. Use multi-threading and/or compute shaders to perform physics updates and simulate many particles (I want to push the bounds and see how high I can get)
2. Change the billboard or sphere-mesh way of rendering the fluid to get a realistic fluid render
3. Add support for interactions with non-particle meshes, i.e. dropping objects into the fluid and having them interact

## Progress
### 01 - Particles Rendered, Interactive Windows
After a long time developing a more general-purpose rendering engine, I finally have a starting point for this project. Using Dear ImGui for the debug windows, I have an interface to interact with important parameters.
Currently, the particles are just given a random initial velocity with gravity acting on them and basic collision with the walls of the window.

![Demonstration of Interactive Window](pics/Milestone1-InteractiveParticleSystem.gif)

Next step is to apply basic collision detection between the particles.

### 02 - Particle Collisions, Introduce Forces With Mouse Input
I implemented a brute-force, $O(n^2)$ algorithm to detect collisions between particles. Currently, it can only handle about 300 particles at 60 fps. I don't have plans to speed this up yet since I will be removing the particle collisions 
for the fluid simulation, but the spatial hashing method I will use in the fluid simulation will be applicable to this kind of collision handling as well.

I also implemented simple push and pull interaction forces when the mouse is left or right clicked. All this does is apply an acceleration to particles in the interaction radius whose strength is determined by how close to the mouse the particle is
and how fast the particle is moving.

$$ \vec{a}_{interaction} = (\frac{\vec{r_i}-\vec{r_p}}{|\vec{r_i}-\vec{r_p}|}S - \vec{v_p})W(\vec{r_i}-\vec{r_p}, h), $$
  
$$
W(\vec{r}, h) = 
	\begin{cases}
		1 - \frac{|\vec{r}|}{h} & 0 \leq |\vec{r}| \leq h \\
		0 & \text{otherwise} 
	\end{cases}
$$

Where:

$\vec{r_i} \rightarrow$ position of interaction source,

$\vec{r_p} \rightarrow$ position of particle,

$\vec{v_p} \rightarrow$ velocity of particle,

$h \rightarrow$ interaction radius, and

$S \rightarrow$ interaction strength constant


![Demonstration of Mouse Interaction and Particle Collisions](pics/Milestone2-ParticleCollisions.gif)
