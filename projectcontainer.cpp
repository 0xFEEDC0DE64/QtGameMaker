#include "projectcontainer.h"

QDataStream &operator<<(QDataStream &ds, const Sprite &sprite)
{
    ds << sprite.name;
    {
        int pixmaps = sprite.pixmaps.size();
        ds << pixmaps;
    }
    for (const auto &pixmap : sprite.pixmaps)
        ds << pixmap;
    ds << sprite.origin.x;
    ds << sprite.origin.y;
    ds << sprite.preciseCollisionChecking;
    ds << sprite.separateCollisionMasks;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Sprite &sprite)
{
    ds >> sprite.name;
    {
        int pixmaps;
        ds >> pixmaps;

        for (int i = 0; i < pixmaps; i++)
        {
            QPixmap pixmap;
            ds >> pixmap;
            sprite.pixmaps.emplace_back(std::move(pixmap));
        }
    }
    ds >> sprite.origin.x;
    ds >> sprite.origin.y;
    ds >> sprite.preciseCollisionChecking;
    ds >> sprite.separateCollisionMasks;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Sound &sound)
{
    ds << sound.name;
    ds << sound.type;
    ds << sound.path;
    ds << sound.effects.chorus;
    ds << sound.effects.flanger;
    ds << sound.effects.gargle;
    ds << sound.effects.echo;
    ds << sound.effects.reverb;
    ds << sound.volume;
    ds << sound.pan;
    ds << sound.preload;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Sound &sound)
{
    ds >> sound.name;
    ds >> sound.type;
    ds >> sound.path;
    ds >> sound.effects.chorus;
    ds >> sound.effects.flanger;
    ds >> sound.effects.gargle;
    ds >> sound.effects.echo;
    ds >> sound.effects.reverb;
    ds >> sound.volume;
    ds >> sound.pan;
    ds >> sound.preload;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Background &background)
{
    ds << background.name;
    ds << background.pixmap;
    ds << background.tileset;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Background &background)
{
    ds >> background.name;
    ds >> background.pixmap;
    ds >> background.tileset;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Path &path)
{
    ds << path.name;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Path &path)
{
    ds >> path.name;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Script &script)
{
    ds << script.name;
    ds << script.script;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Script &script)
{
    ds >> script.name;
    ds >> script.script;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const Font &font)
{
    ds << font.name;
    ds << font.font;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Font &font)
{
    ds >> font.name;
    ds >> font.font;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, const ProjectContainer &project)
{
    {
        int sprites = project.sprites.size();
        ds << sprites;
        for (const auto &sprite : project.sprites)
            ds << sprite;
    }
    {
        int sounds = project.sounds.size();
        ds << sounds;
        for (const auto &sound : project.sounds)
            ds << sound;
    }
    {
        int backgrounds = project.backgrounds.size();
        ds << backgrounds;
        for (const auto &background : project.backgrounds)
            ds << background;
    }
    {
        int paths = project.paths.size();
        ds << paths;
        for (const auto &path : project.paths)
            ds << path;
    }
    {
        int scripts = project.scripts.size();
        ds << scripts;
        for (const auto &script : project.scripts)
            ds << script;
    }
    {
        int fonts = project.fonts.size();
        ds << fonts;
        for (const auto &font : project.fonts)
            ds << font;
    }
    return ds;
}

QDataStream &operator>>(QDataStream &ds, ProjectContainer &project)
{
    {
        int sprites;
        ds >> sprites;

        for (int i = 0; i < sprites; i++)
        {
            Sprite sprite;
            ds >> sprite;
            project.sprites.emplace_back(std::move(sprite));
        }
    }
    {
        int sounds;
        ds >> sounds;

        for (int i = 0; i < sounds; i++)
        {
            Sound sound;
            ds >> sound;
            project.sounds.emplace_back(std::move(sound));
        }
    }
    {
        int backgrounds;
        ds >> backgrounds;

        for (int i = 0; i < backgrounds; i++)
        {
            Background background;
            ds >> background;
            project.backgrounds.emplace_back(std::move(background));
        }
    }
    {
        int paths;
        ds >> paths;

        for (int i = 0; i < paths; i++)
        {
            Path path;
            ds >> path;
            project.paths.emplace_back(std::move(path));
        }
    }
    {
        int scripts;
        ds >> scripts;

        for (int i = 0; i < scripts; i++)
        {
            Script script;
            ds >> script;
            project.scripts.emplace_back(std::move(script));
        }
    }
    {
        int fonts;
        ds >> fonts;

        for (int i = 0; i < fonts; i++)
        {
            Font font;
            ds >> font;
            project.fonts.emplace_back(std::move(font));
        }
    }
    return ds;
}

