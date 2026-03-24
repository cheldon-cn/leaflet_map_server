class GridRegion {
    static Calculate(winRect, regionCode) {
        if (regionCode = 0) {
            return {x: winRect.x, y: winRect.y, w: winRect.w, h: winRect.h}
        }
        
        regionCode := Utils.Clamp(regionCode, 1, 9)
        
        thirdW := winRect.w / 3
        thirdH := winRect.h / 3
        
        col := Mod(regionCode - 1, 3)
        row := (regionCode - 1) // 3
        
        x := winRect.x + col * thirdW
        y := winRect.y + row * thirdH
        
        return {x: Round(x), y: Round(y), w: Round(thirdW), h: Round(thirdH)}
    }
    
    static GetRegionName(regionCode) {
        names := Map(
            0, "全屏",
            1, "左上",
            2, "中上",
            3, "右上",
            4, "左中",
            5, "中中",
            6, "右中",
            7, "左下",
            8, "中下",
            9, "右下"
        )
        
        return names.Has(regionCode) ? names[regionCode] : "未知"
    }
    
    static GetAllRegions(winRect) {
        regions := Map()
        
        regions[0] := {x: winRect.x, y: winRect.y, w: winRect.w, h: winRect.h, name: "全屏"}
        
        thirdW := winRect.w / 3
        thirdH := winRect.h / 3
        
        loop 9 {
            regionCode := A_Index
            col := Mod(regionCode - 1, 3)
            row := (regionCode - 1) // 3
            
            x := winRect.x + col * thirdW
            y := winRect.y + row * thirdH
            
            regions[regionCode] := {
                x: Round(x),
                y: Round(y),
                w: Round(thirdW),
                h: Round(thirdH),
                name: this.GetRegionName(regionCode)
            }
        }
        
        return regions
    }
    
    static GetRelativePosition(winRect, x, y) {
        relX := (x - winRect.x) / winRect.w
        relY := (y - winRect.y) / winRect.h
        
        col := Floor(relX * 3)
        row := Floor(relY * 3)
        
        col := Utils.Clamp(col, 0, 2)
        row := Utils.Clamp(row, 0, 2)
        
        return row * 3 + col + 1
    }
    
    static GetRegionFromPosition(winRect, x, y) {
        if (x < winRect.x || x > winRect.x + winRect.w ||
            y < winRect.y || y > winRect.y + winRect.h) {
            return 0
        }
        
        return this.GetRelativePosition(winRect, x, y)
    }
}
