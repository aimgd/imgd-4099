## Assignment 5

In this assignment I implemented Langton's ants with 2 modified types of Vants. The original Vant (in red), follows the rules: 
	- Turn 90 degrees CCW if there are pheramones
	- Turn 90 degrees CW if there aren't pheramones
and makes it so that the presence of pheramones in a given cell are the opposite of when they entered the cell (cells with pheramones are removed and cells without have pheramones added). The new types of Vants are the green one, which does a search in a N by N rectangle to find the nearest pheramone and eats it. When it eats a certain amount, it will switch to a blue Vant. The blue Vant will deposit the amount of pheramones the green one ate and will be "accelerated" in a path that is tangent to the average direction vector of pheramones around it- or in essence, tries to path "around" pheramones. The blue one has a "velcoity" accumulator and travels in the velocity vector direction normalized and quantized to the grid. Once the blue Vant deploys all of its pheramones it turns back into a green Vant, repeating the cycle. This behaviour creates a pretty interesting simulation where the blue/green vants kinda redistribute the pheramone around the screen creating some more complex behaviour than the default red Vant.