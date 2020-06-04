import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
from matplotlib.lines import Line2D
from cycler import cycler

def plot_4d_contour(data, colour_array):

    fig = plt.figure()
    ax = Axes3D(fig)

    #ax.set_xlabel('w1')
    #ax.set_ylabel('w2')
    #ax.set_zlabel('b')

    ax.scatter(data[:,0], data[:,1], data[:,2], c=colour_array, cmap='plasma')

    plt.show()

def plot_3d_scatter(data):

    fig = plt.figure()
    ax = Axes3D(fig)

    ax.set_xlabel('w1')
    ax.set_ylabel('w2')
    ax.set_zlabel('w3')

    ax.scatter(data[:,0], data[:,1], data[:,2])

    plt.show()

def plot_2d_scatter(data):

    plt.scatter(data[:,0], data[:,1])
    plt.show()

def plot_2d_scatter_colour(data, colour_array, vmin='', vmax=''):

    if vmin != '':
        plt.scatter(data[:,0], data[:,1], c=colour_array, cmap='plasma', vmin=vmin, vmax=vmax)
    else:
        plt.scatter(data[:,0], data[:,1], c=colour_array, cmap='plasma')

    plt.colorbar()
    plt.show()

def plot_2d_contour(X, Y, Z, xlabel='', ylabel='', colourbar_title=''):

    plt.contourf(X, Y, Z, 100, cmap='plasma')

    plt.xlabel(xlabel)
    plt.ylabel(ylabel, rotation='horizontal')

    cbar = plt.colorbar()
    cbar.ax.set_ylabel(colourbar_title, rotation='horizontal')
    cbar.ax.get_yaxis().set_label_coords(5.5, 0.5)

    plt.show()

def plot_2d_line(data, xlabel='', ylabel='', xaxis='', yaxis='', 
                 legend_labels='', plot_colours=''):

    if len(data.shape) == 3:
        for i in range(data.shape[0]):
            if plot_colours != '':
                plt.plot(data[i,:,0], data[i,:,1], color=plot_colours[i])
            else:
                plt.plot(data[i,:,0], data[i,:,1])
    else:
        plt.plot(data[:,0], data[:,1])

    #Axis labels
    plt.xlabel(xlabel)
    plt.ylabel(ylabel, rotation='horizontal')

    #Axis ranges
    if xaxis != '':
        plt.gca().set_xlim(xaxis)
    if yaxis != '':
        plt.gca().set_ylim(yaxis)

    #Build legend
    if legend_labels != '':
        legend_elements = []
        for i in range(len(data)):
            legend_elements.append(Line2D([], [], color=plot_colours[i], 
                                          label=legend_labels[i]))
        plt.legend(handles=legend_elements, loc="upper center", bbox_to_anchor=(0.5, 1.13))

    plt.gca().yaxis.set_label_coords(-0.1,0.575)

    plt.show()

#Plots 2d wireframe mesh
def plot_2d_wireframe(xmesh, ymesh, xmin, xmax, ymin, ymax, xlabel='', ylabel=''):

    #Plot axis
    xdiff = xmax - xmin
    ydiff = ymax - ymin
    border_perc = 0.01
    xbord = border_perc * xdiff
    ybord = border_perc * ydiff

    plt.axis([xmin - xbord, xmax + xbord, ymin - ybord, ymax + ybord])
    plt.gca().set_aspect('equal')

    xlen = len(xmesh[0])
    ylen = len(ymesh)

    xmeshmin = xmesh[0,0]
    xmeshmax = xmesh[0,-1]
    ymeshmin = ymesh[0,0]
    ymeshmax = ymesh[-1,0]

    num_increments_per_unit = 10
    num_x_increments = int(xdiff * num_increments_per_unit)
    num_y_increments = int(ydiff * num_increments_per_unit)

    #Find indices to plot
    xindices = generate_incremented_indices(0, xlen, num_increments=num_x_increments)
    yindices = generate_incremented_indices(0, ylen, num_increments=num_y_increments)

    #Create lines
    vertical_lines = np.empty((len(xindices), ylen, 2))
    horizontal_lines = np.empty((len(yindices), xlen, 2))

    for i, index in enumerate(yindices):
        horizontal_lines[i, :, 0] = xmesh[index, :]
        horizontal_lines[i, :, 1] = ymesh[index, :]

    for i, index in enumerate(xindices):
        vertical_lines[i, :, 0] = xmesh[:, index]
        vertical_lines[i, :, 1] = ymesh[:, index]

    #Plot vertical lines
    for line in vertical_lines:
        plt.plot(line[:,0], line[:,1], color='b')

    #Plot horizontal lines
    for line in horizontal_lines:
        plt.plot(line[:,0], line[:,1], color='b')

    #Calculate number of ticks
    xticks = np.linspace(xmin, xmax, num_x_increments+1)
    yticks = np.linspace(ymin, ymax, num_y_increments+1)
    plt.gca().set_xticks(xticks)
    plt.gca().set_yticks(yticks)

    plt.gca().yaxis.set_label_coords(-0.13,0.53)

    plt.xlabel(xlabel)
    plt.ylabel(ylabel, rotation='horizontal')

    plt.grid()

    plt.show()

def plot_1d_increments(data, num_increments, xlabel='', xaxis='', legend_labels='',
                       plot_colours=['b', 'r', 'g']):

    #Only plot num_increments amount of data
    #Calculate the indices of the data array to plot
    indices = [0]
    data_increment = len(data[0]) / num_increments
    while len(indices) < num_increments + 1:
        indices.append(indices[-1] + data_increment)

    indices = np.array(indices, dtype=int)
    indices[-1] = indices[-1] - 1

    #Gather data to plot
    plotted_data = []

    #Calculate y values for the different data sets
    y_increment = 1 / (len(data) + 1)
    y_center_values = [y_increment]
    for y in range(len(data)-1):
        y_center_values.append(y_center_values[-1] + y_increment)
    y_center_values.reverse()
    y_range = 0.2 

    #Plot colours
    assert len(plot_colours) >= len(data), "Not enough colours for plotting!"

    #Prepare data to be plotted
    for i in range(len(data)):

        for j in indices:
            plotted_data.append(data[i, j])

        y_min = y_center_values[i] - y_range/2
        y_max = y_center_values[i] + y_range/2

        for d in plotted_data:
            plt.axvline(x=d, ymin=y_min, ymax=y_max, color=plot_colours[i])

        plotted_data = []

    #Axis settings
    plt.gca().set_ylim([-1., 1.])
    plt.gca().get_yaxis().set_visible(False)

    plt.xlabel(xlabel)

    #Build legend
    if legend_labels != '':
        legend_elements = []
        for i in range(len(data)):
            legend_elements.append(Line2D([], [], color=plot_colours[i], linestyle='None',
                                          label=legend_labels[i], marker='|', markersize=10))
        plt.legend(handles=legend_elements, loc="upper center", bbox_to_anchor=(0.5, 1.13))

    #Plot
    plt.grid()
    plt.show()

#Return num_increments equal spaced data 
def generate_incremented_data(data, num_increments):

    #Calculate indices in data to save
    incremented_indices = generate_incremented_indices(imin=0, imax=len(data), 
                                                       num_increments=num_increments)

    #Save data
    incremented_data = np.zeros((num_increments + 1))

    for i in range(len(incremented_indices)):
        incremented_data[i] = data[incremented_indices[i]]

    print(data)
    print(incremented_data)

    return incremented_data

def generate_incremented_indices(imin, imax, num_increments=''):

    incremented_indices = [imin]

    #if value_increment != '':

    #    num_increments = 1
    #    cumm_value = 0.0

    #    while cumm_value < values[-1]:
    #        num_increments += 1
    #        cumm_value += value_increment

    increment = (imax - imin) / num_increments

    while len(incremented_indices) < num_increments + 1:
        incremented_indices.append(incremented_indices[-1] + increment)

    incremented_indices = np.array(incremented_indices, dtype=int)
    incremented_indices[-1] = incremented_indices[-1] - 1

    return incremented_indices
