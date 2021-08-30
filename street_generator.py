#!/usr/bin/env python3
import sys
import re

# YOUR CODE GOES HERE


def read_input(line, street_status):
    if re.findall(r'^[g]$', line):
        street_status.gen()
        return
    add_pointer = re.findall(
        r'[a](?:\s)+"(?:\s)*\w+(?:(?:\s)+\w+)*"(?:\s)+\({1}(?:\-)?[0-9]+(?:\.[0-9]+)?,(?:\s)*(?:\-)?[0-9]+(?:\.[0-9]+)?\){1}(?:(?:\s)*\({1}(?:\-)?[0-9]+(?:\.[0-9]+)?,(?:\-)?[0-9]+(?:\.[0-9]+)?\){1})+',
        line
    )
    change_pointer = re.findall(
        r'[c](?:\s)+"(?:\s)*\w+(?:(?:\s)+\w+)*"(?:\s)+\({1}(?:\-)?[0-9]+(?:\.[0-9]+)?,(?:\s)*(?:\-)?[0-9]+(?:\.[0-9]+)?\){1}(?:(?:\s)*\({1}(?:\-)?[0-9]+(?:\.[0-9]+)?,(?:\-)?[0-9]+(?:\.[0-9]+)?\){1})+',
        line
    )
    remove_pointer = re.findall(
        r'[r](?:\s)+"(?:\s)*\w+(?:(?:\s)+\w+)*"',
        line
    )
    if add_pointer:
        if len(add_pointer[0]) == len(line):
            street_name = get_street_name(line)
            coord_num_list = get_num_list(line)
            street_status.add(street_name, coord_num_list)
            return

    if change_pointer:
        if len(change_pointer[0]) == len(line):
            street_name = get_street_name(line)
            coord_num_list = get_num_list(line)
            street_status.change(street_name, coord_num_list)
            return

    if remove_pointer:
        if len(remove_pointer[0]) == len(line):
            street_name = get_street_name(line)
            street_status.remove(street_name)
            return
    # sys.stderr.write("Error: The command format is not correct. \n")
    print('Error: Incorrect input format', file=sys.stderr)


class Node:         # get a single point
    def __init__(self, x, y):
        self.x = float(x)
        self.y = float(y)

    def __repr__(self):
        return '({0:.2f},{1:.2f})'.format(self.x, self.y)

    def __eq__(self,other):
        if not isinstance(other,Node):
            return NotImplemented
        return (self.x,self.y) == (other.x, other.y)


def are_vertex_the_same(vertex1, vertex2):
    if round(vertex1.x, 4) == round(vertex2.x, 4) and round(vertex1.y, 4) == round(vertex2.y, 4):
        return True
    else:
        return False


class LineSegment:          # get Line segment
    def __init__(self, node1, node2):
        self.node1 = node1
        self.node2 = node2

    def __repr__(self):
        return '({},{})'.format(self.node1, self.node2)


def are_line_segment_the_same(line_segment1, line_segment2):
    if line_segment1.node1 == line_segment2.node1 and line_segment1.node2 == line_segment2.node2:
        return True
    elif line_segment1.node1 == line_segment2.node2 and line_segment1.node2 == line_segment2.node1:
        return True
    else:
        return False


def line_status(line_segment):       # get slope and intercept from line segment
    line_status_dict = {}
    x1 = line_segment.node1.x
    y1 = line_segment.node1.y
    x2 = line_segment.node2.x
    y2 = line_segment.node2.y

    if x1 == x2:
        return line_status_dict
    else:
        line_status_dict['slope'] = (y2 - y1)/(x2 - x1)
        line_status_dict['intercept'] = (x2*y1 - x1*y2)/(x2 - x1)
        return line_status_dict


def is_intersection_on_line_segments(line_segment, intersection):
    x1 = line_segment.node1.x
    x2 = line_segment.node2.x
    y1 = line_segment.node1.y
    y2 = line_segment.node2.y
    x_range = [x1,x2]
    x_range.sort()
    y_range = [y1,y2]
    y_range.sort()
    if round(x_range[0],4)<=round(intersection.x,4)<=round(x_range[1],4) and round(y_range[0],4)<=round(intersection.y,4)<=round(y_range[1],4):
        return True
    else:
        return False


def add_to_node_list(node, node_list):
    pointer = 1
    for exist_node in node_list:
        if are_vertex_the_same(exist_node, node):
            pointer = 0
    if pointer == 1:
        node_list.append(node)


def add_to_street_vex_dict(street_vex_dict,street_name,vex_list):

    if street_name in street_vex_dict:

        for vex in vex_list:
            pointer = 1
            for exist_vex in street_vex_dict[street_name]:
                if are_vertex_the_same(vex, exist_vex):
                    pointer = 0
            if pointer == 1:
                street_vex_dict[street_name].append(vex)

    else:
        street_vex_dict[street_name] = vex_list


def add_to_edge_list(edge_list,line_segment):
    pointer = 1
    for exist_edge in edge_list:
        if are_line_segment_the_same(line_segment,exist_edge):
            pointer = 0
    if pointer == 1:
        edge_list.append(line_segment)


def get_intersection_list_street_vertex_dict(line_segment_1,line_segment_2,intersection_list,street_vex_dict,street_name_1,street_name_2):
    x1 = line_segment_1.node1.x
    x2 = line_segment_2.node1.x
    line_segment_1_status = line_status(line_segment_1)
    line_segment_2_status = line_status(line_segment_2)
    if len(line_segment_1_status) == 0:
        if len(line_segment_2_status) != 0:
            k2 = line_segment_2_status['slope']
            b2 = line_segment_2_status['intercept']
            intersection_x = x1
            intersection_y = k2 * x1 + b2
            intersection_pre = Node(intersection_x,intersection_y)
            if is_intersection_on_line_segments(line_segment_1, intersection_pre) and is_intersection_on_line_segments(line_segment_2,intersection_pre):
                intersection = intersection_pre
                add_to_node_list(intersection,intersection_list)
                vex_list_1 = [line_segment_1.node1,intersection,line_segment_1.node2]
                vex_list_2 = [line_segment_2.node1,intersection,line_segment_2.node2]
                add_to_street_vex_dict(street_vex_dict,street_name_1,vex_list_1)
                add_to_street_vex_dict(street_vex_dict,street_name_2,vex_list_2)

    if len(line_segment_2_status) == 0:
        if len(line_segment_1_status) != 0:
            k1 = line_segment_1_status['slope']
            b1 = line_segment_1_status['intercept']
            intersection_x = x2
            intersection_y = k1 * x2 + b1
            intersection_pre = Node(intersection_x,intersection_y)
            if is_intersection_on_line_segments(line_segment_1, intersection_pre) and is_intersection_on_line_segments(line_segment_2,intersection_pre):
                intersection = intersection_pre
                add_to_node_list(intersection,intersection_list)
                vex_list_1 = [line_segment_1.node1,intersection,line_segment_1.node2]
                vex_list_2 = [line_segment_2.node1,intersection,line_segment_2.node2]
                add_to_street_vex_dict(street_vex_dict,street_name_1,vex_list_1)
                add_to_street_vex_dict(street_vex_dict,street_name_2,vex_list_2)

    if len(line_segment_1_status) != 0:
        if len(line_segment_2_status) != 0:
            k1 = line_segment_1_status['slope']
            b1 = line_segment_1_status['intercept']
            k2 = line_segment_2_status['slope']
            b2 = line_segment_2_status['intercept']

            if k1 != k2:
                intersection_x = (b2 - b1) / (k1 - k2)
                intersection_y = (k1*b2 - k2*b1) / (k1 - k2)
                intersection_pre = Node( intersection_x, intersection_y )
                if is_intersection_on_line_segments(line_segment_1, intersection_pre) and is_intersection_on_line_segments(line_segment_2, intersection_pre):
                    intersection = intersection_pre
                    add_to_node_list( intersection, intersection_list )
                    vex_list_1 = [line_segment_1.node1, intersection, line_segment_1.node2]
                    vex_list_2 = [line_segment_2.node1, intersection, line_segment_2.node2]
                    add_to_street_vex_dict(street_vex_dict, street_name_1, vex_list_1)
                    add_to_street_vex_dict(street_vex_dict, street_name_2, vex_list_2)

            if k1 == k2:
                if b1 == b2:
                    point1_1 = line_segment_1.node1
                    point1_2 = line_segment_1.node2
                    point2_1 = line_segment_2.node1
                    point2_2 = line_segment_2.node2

                    times = 0
                    for point_a in [point1_1, point1_2]:
                        if is_intersection_on_line_segments(line_segment_2,point_a):
                            add_to_node_list(point_a, intersection_list)
                            times += 1

                    for point_b in [point2_1, point2_2]:
                        if is_intersection_on_line_segments(line_segment_1,point_b):
                            add_to_node_list(point_b, intersection_list)
                            times += 1

                    if times > 0:
                        vex_list_1 = [point1_1,point1_2]
                        for intersection in intersection_list:
                            add_to_node_list(intersection, vex_list_1)
                        if point1_1.x < point1_2.x:
                            sorted_vex_list_1 = sorted(vex_list_1,key=lambda item:item.x)
                        if point1_1.x > point1_2.x:
                            sorted_vex_list_1 = sorted(vex_list_1, key=lambda item: item.x, reverse=True)

                        vex_list_2 = [point2_1,point2_2]
                        for intersection in intersection_list:
                            add_to_node_list(intersection, vex_list_2)
                        if point2_1.x < point2_2.x:
                            sorted_vex_list_2 = sorted(vex_list_2,key=lambda item:item.x)
                        if point2_1.x > point2_2.x:
                            sorted_vex_list_2 = sorted(vex_list_2,key=lambda item:item.x, reverse=True)

                        add_to_street_vex_dict(street_vex_dict, street_name_1, sorted_vex_list_1)
                        add_to_street_vex_dict(street_vex_dict, street_name_2, sorted_vex_list_2)

    if len(line_segment_1_status) == 0 and len(line_segment_2_status) == 0:
        if x1 == x2:
            point1_1 = line_segment_1.node1
            point1_2 = line_segment_1.node2
            point2_1 = line_segment_2.node1
            point2_2 = line_segment_2.node2

            times = 0
            for point_a in [point1_1, point1_2]:
                if is_intersection_on_line_segments(line_segment_2, point_a):
                    add_to_node_list( point_a, intersection_list )
                    times += 1

            for point_b in [point2_1, point2_2]:
                if is_intersection_on_line_segments( line_segment_1, point_b ):
                    add_to_node_list( point_b, intersection_list)
                    times += 1

            if times > 0:
                vex_list_1 = [point1_1, point1_2]
                for intersection in intersection_list:
                    add_to_node_list( intersection, vex_list_1 )
                if point1_1.y < point1_2.y:
                    sorted_vex_list_1 = sorted( vex_list_1, key=lambda item: item.y )
                if point1_1.y > point1_2.y:
                    sorted_vex_list_1 = sorted( vex_list_1, key=lambda item: item.y, reverse=True )

                vex_list_2 = [point2_1, point2_2]
                for intersection in intersection_list:
                    add_to_node_list( intersection, vex_list_2 )
                if point2_1.y < point2_2.y:
                    sorted_vex_list_2 = sorted( vex_list_2, key=lambda item: item.y )
                if point2_1.y > point2_2.y:
                    sorted_vex_list_2 = sorted( vex_list_2, key=lambda item: item.y, reverse=True )

                add_to_street_vex_dict(street_vex_dict, street_name_1, sorted_vex_list_1)
                add_to_street_vex_dict(street_vex_dict, street_name_2, sorted_vex_list_2)


def get_edge_list(edge_list,intersection_list,street_vex_dict):
    if len(street_vex_dict) > 0:
        for key, value in street_vex_dict.items():
            for i in range(len(value)-1):
                for intersect in intersection_list:
                    if are_vertex_the_same(value[i],intersect) or are_vertex_the_same(value[i+1],intersect):
                        edge_line_segment = LineSegment(value[i],value[i+1])
                        if not are_vertex_the_same(value[i], value[i+1]):
                            add_to_edge_list(edge_list,edge_line_segment)


def street_vex_to_num_vex_list(street_vex_dict):
    num_vex_list = []
    for key, value in street_vex_dict.items():
        for vex in value:
            add_to_node_list(vex, num_vex_list)
    return num_vex_list


def print_output(num_vex_list, edge_list):
    # print("V = {")
    # for i, vertex in enumerate(num_vex_list):
    #     print("  {}: {}".format(i, vertex))
    # print("}")
    sys.stdout.write("V {0}\n".format(len(num_vex_list)))
    sys.stdout.flush()

    # print("E = {")
    # if len(edge_list)>0:
    #     for edge_line_seg in edge_list[0:len(edge_list)-1]:
    #         for vex in num_vex_list:
    #             if are_vertex_the_same(edge_line_seg.node1, vex):
    #                 index1 = num_vex_list.index(vex)
    #             if are_vertex_the_same(edge_line_seg.node2, vex):
    #                 index2 = num_vex_list.index(vex)
    #
    #         print("  <{0},{1}>,".format(index1, index2))
    #
    #     last_edge_line_seg = edge_list[-1]
    #     for vex in num_vex_list:
    #         if are_vertex_the_same(last_edge_line_seg.node1,vex):
    #             index3 = num_vex_list.index(vex)
    #         if are_vertex_the_same(last_edge_line_seg.node2,vex):
    #             index4 = num_vex_list.index(vex)
    #     print("  <{0},{1}>".format(index3, index4))
    #
    # print("}\n")
    sys.stdout.write("E {")
    if len(edge_list)>0:
        for edge_line_seg in edge_list[0:len(edge_list)-1]:
            for vex in num_vex_list:
                if are_vertex_the_same(edge_line_seg.node1, vex):
                    index1 = num_vex_list.index(vex)
                if are_vertex_the_same(edge_line_seg.node2, vex):
                    index2 = num_vex_list.index(vex)

            sys.stdout.write("<{0},{1}>,".format(index1,index2))
        last_edge_line_seg = edge_list[-1]
        for vex in num_vex_list:
            if are_vertex_the_same(last_edge_line_seg.node1,vex):
                index3 = num_vex_list.index(vex)
            if are_vertex_the_same(last_edge_line_seg.node2,vex):
                index4 = num_vex_list.index(vex)

        sys.stdout.write("<{0},{1}>".format(index3,index4))

    sys.stdout.write("}\n")
    sys.stdout.flush()


def get_street_name(line):
    street_name = re.findall(r'"([^"]*)"', line)[0]
    street_name = street_name.lower()
    return street_name


def make_multiple_intersections_on_line_in_order(street_dict, street_vex_dict, intersection_list):
    for key, value in street_dict.items():
        for i in range(len(value)-1):
            line_segment = LineSegment(value[i],value[i+1])
            times = 0

            for intersection in intersection_list:
                if is_intersection_on_line_segments(line_segment,intersection):
                    if not are_vertex_the_same(value[i],intersection) and not are_vertex_the_same(value[i+1],intersection):
                        times += 1
            if times > 1:
                index1 = street_vex_dict[key].index(value[i])
                index2 = index1 + times + 2
                target_vex_list = street_vex_dict[key][index1:index2]
                if value[i].x == value[i+1].x:
                    if value[i].y < value[i+1].y:
                        street_vex_dict[key][index1:index2] = sorted(target_vex_list, key=lambda item: item.y)
                    if value[i].y > value[i+1].y:
                        street_vex_dict[key][index1:index2] = sorted(target_vex_list, key=lambda item: item.y,
                                                                     reverse=True)
                else:
                    if value[i].x < value[i+1].x:
                        street_vex_dict[key][index1:index2] = sorted(target_vex_list, key=lambda item: item.x)
                    if value[i].x > value[i+1].x:
                        street_vex_dict[key][index1:index2] = sorted(target_vex_list, key=lambda item: item.x,
                                                                     reverse=True)


def get_num_list(line):
    coord_num_list = []
    coord_str_list = re.findall(r'\((?:\-)?[0-9]+(?:\.[0-9]+)?,(?:\-)?[0-9]+(?:\.[0-9]+)?\)', line)
    for i in coord_str_list:
        coord_val = re.findall(r'(?:\-)?[0-9]+(?:\.[0-9]+)?', i)
        coord_node = Node(coord_val[0], coord_val[1])
        coord_num_list.append(coord_node)
    for i in range(len(coord_num_list)-1):
        if coord_num_list[i] == coord_num_list[i+1]:
            print('Incorrect input format', file=sys.stderr)
            return
    return coord_num_list


def get_graph(street_dict):
    street_vex_dict = {}
    edge_list = []
    intersection_list = []
    street_name_list = list(street_dict.keys())

    for i in range(len(street_name_list)):
        street_name_selected = street_name_list[i]
        street_nodes_selected = street_dict[street_name_selected]
        line_segment_list_selected = []
        for j in range(len(street_nodes_selected)-1):
            line_segment_list_selected.append(LineSegment(street_nodes_selected[j],street_nodes_selected[j+1]))
            for street_name_other in street_name_list[i+1:]:
                street_nodes_other = street_dict[street_name_other]
                line_segment_list_other = []
                for k in range(len(street_nodes_other)-1):
                    line_segment_list_other.append(LineSegment(street_nodes_other[k],street_nodes_other[k+1]))

                    for line_segment_selected in line_segment_list_selected:
                        for line_segment_other in line_segment_list_other:
                            get_intersection_list_street_vertex_dict(line_segment_selected, line_segment_other,intersection_list,
                                                                      street_vex_dict,street_name_selected,street_name_other)

    make_multiple_intersections_on_line_in_order(street_dict,street_vex_dict,intersection_list)
    get_edge_list(edge_list,intersection_list,street_vex_dict)
    num_vex_list = street_vex_to_num_vex_list(street_vex_dict)
    print_output(num_vex_list, edge_list)

    #return edge_list,intersection_list,street_vex_dict # temp


class Streets:
    def __init__(self):
        self.street_dict = {}

    def add(self, street_name, coord_num_list):
        if street_name in self.street_dict:
            print("Error: street currently exists.", file=sys.stderr)
            # sys.stderr.write("Error: 'a' specified for a street that already exist.")
        else:
            self.street_dict[street_name] = coord_num_list

    def change(self, street_name, coord_num_list):
        if street_name in self.street_dict:
            self.street_dict[street_name] = coord_num_list
        else:
            sys.stderr.write("Error: 'c' or 'r' specified for a street that does not exist.")

    def remove(self, street_name):
        if street_name in self.street_dict:
            del self.street_dict[street_name]
        else:
            sys.stderr.write("Error: 'c' or 'r' specified for a street that does not exist.")

    def gen(self):
        """generate a graph"""
        get_graph(self.street_dict)


def main():
    # YOUR MAIN CODE GOES HERE

    # sample code to read from stdin.
    # make sure to remove all spurious print statements as required
    # by the assignment
    street_status = Streets()

    while True:
        try:
            line = sys.stdin.readline()
            if line == "":
                break

            line = line.strip('\n')

        except EOFError:
            break

        read_input(line, street_status)

    # return exit code 0 on successful termination
    sys.exit(0)


if __name__ == "__main__":
    main()
