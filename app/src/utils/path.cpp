#include "roah/distb/utils/path.hpp"

std::filesystem::path
roah::distb::utils::makeAbsolutePath(const std::filesystem::path & path)
{
    return std::filesystem::absolute(path).make_preferred();
}

bool
roah::distb::utils::isSubDirectory(const std::filesystem::path & parent,
                                   const std::filesystem::path & child,
                                   const bool                    when_same)
{
    if (parent == child)
    {
        return when_same;
    }
    if (child.has_parent_path())
    {
        return isSubDirectory(parent, child.parent_path(), true);
    }
    return false;
}
