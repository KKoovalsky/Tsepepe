/**
 * @file        yolo_interface.hpp
 * @brief       Yolo interface.
 */
#ifndef YOLO_INTERFACE_HPP
#define YOLO_INTERFACE_HPP

namespace Tsepepe
{
struct YoloInterface
{
    virtual void do_stuff() = 0;
    virtual ~YoloInterface() = default;
};
} // namespace Tsepepe

#endif /* YOLO_INTERFACE_HPP */
