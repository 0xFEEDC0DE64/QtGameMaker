#pragma once

class ProjectChangesApplier
{
public:
    virtual ~ProjectChangesApplier() = default;
    virtual bool applyChanges() = 0;
};
