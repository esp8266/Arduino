package processing.app.debug;

import java.io.File;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import processing.app.helpers.filefilters.OnlyDirs;

public class TargetPackage {

	String name;
	File folder;

	Map<String, TargetPlatform> platforms = new HashMap<String, TargetPlatform>();

	public TargetPackage(String _name, File _folder) {
		name = _name;
		folder = _folder;

		String[] platformsList = folder.list(new OnlyDirs());
		for (String platformName : platformsList) {
			File platformFolder = new File(folder, platformName);
			TargetPlatform platform = new TargetPlatform(platformName, platformFolder);
			platforms.put(platformName, platform);
		}
	}

	public Map<String, TargetPlatform> getPlatforms() {
		return platforms;
	}

	public Collection<TargetPlatform> platforms() {
		return platforms.values();
	}

	public TargetPlatform get(String platform) {
		return platforms.get(platform);
	}
	
	public String getName() {
		return name;
	}
}
