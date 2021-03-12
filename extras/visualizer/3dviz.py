#!/usr/bin/env python
'''
ROS rviz script for visualizing quadcopter state in 3D space

You should run this with a shell script:

    % ./fileviz.sh
    % ./blueviz.sh

Copyright (C) 2021 Simon D. Levy

MIT License
'''

NODE_NAME = 'stateviz'

MARKER_COLOR    = 1.0, 0.0, 0.0
MARKER_RESOURCE = 'package://stateviz/arrowhead.stl'
MARKER_START    = 0.08, 0.08, 0.
MARKER_SCALE    = .02

import rospy
from interactive_markers.interactive_marker_server import InteractiveMarkerServer, InteractiveMarker
from visualization_msgs.msg import Marker, InteractiveMarkerControl
from geometry_msgs.msg import Point
from tf.broadcaster import TransformBroadcaster
from tf.transformations import quaternion_from_euler

import numpy as np
import sys
from threading import Thread

import stateviz

# Globals
br = None
euler = None
translat = None

def _errmsg(message):
    sys.stderr.write(message + '\n')
    sys.exit(1)

def normalizeQuaternion(orientation):

    norm = orientation.x**2 + orientation.y**2 + orientation.z**2 + orientation.w**2
    s = norm**(-0.5)
    orientation.x *= s
    orientation.y *= s
    orientation.z *= s
    orientation.w *= s

def frameCallback(msg):

    global br, euler, translat

    if euler is None or translat is None:
        return

    br.sendTransform(translat, quaternion_from_euler(*euler), rospy.Time.now(), 'vehicle_frame', 'map')                

def processFeedback(feedback):

    server.applyChanges()

class ThreeDVisualizer(object):
    '''
    Methods are called automatically by Stateviz.run()
    '''

    def __init__(self, cmdargs, label, outfile=None):

        self.outfile = outfile

    def display(self, x_m, y_m, z_m, theta_deg):

        global euler, translat

        euler = (0, 0, np.radians(theta_deg))

        translat = tuple(v/10. for v in (x_m,y_m,z_m))
 
        if not self.outfile is None:

            self.outfile.write('%+3.3f %+3.3f %3.3f\n' % (x_m, y_m, theta_deg))
            self.outfile.flush()

        return True

def threadFunc():

    # We pass the class, rather than an instance, because Stateviz.run() will create the instance for us
    stateviz.run(ThreeDVisualizer)

if __name__=='__main__':

    global callbackTime

    thread = Thread(target=threadFunc)
    thread.daemon = True
    thread.start()

    rospy.init_node(NODE_NAME)
    br = TransformBroadcaster()
    rospy.Timer(rospy.Duration(0.01), frameCallback)
    server = InteractiveMarkerServer(NODE_NAME)

    vehicleMarker = InteractiveMarker()
    vehicleMarker.header.frame_id = 'vehicle_frame'
    vehicleMarker.pose.position = Point(*MARKER_START)
    vehicleMarker.scale = 1
    vehicleMarker.name = 'quadcopter'
    q = quaternion_from_euler(0, 0, 0)
    vehicleMarker.pose.orientation.x = q[0]
    vehicleMarker.pose.orientation.y = q[1]
    vehicleMarker.pose.orientation.z = q[2] 
    vehicleMarker.pose.orientation.w = q[3]
    normalizeQuaternion(vehicleMarker.pose.orientation)

    vehicleMesh = Marker()
    vehicleMesh.type = Marker.MESH_RESOURCE
    vehicleMesh.mesh_resource = MARKER_RESOURCE
    vehicleMesh.scale.x, vehicleMesh.scale.y, vehicleMesh.scale.z = (tuple([vehicleMarker.scale*MARKER_SCALE]))*3
    vehicleMesh.color.r, vehicleMesh.color.g, vehicleMesh.color.b = MARKER_COLOR
    vehicleMesh.color.a = 1.0

    vehicleControl =  InteractiveMarkerControl()
    vehicleControl.always_visible = True
    vehicleControl.markers.append(vehicleMesh)

    vehicleMarker.controls.append(vehicleControl)

    server.insert(vehicleMarker, processFeedback)

    server.applyChanges()

    #rospy.spin()

    while not rospy.core.is_shutdown():
        print('*********************')
        rospy.rostime.wallsleep(0.5)
