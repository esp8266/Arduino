package processing.app;

public class ObjectUtil {
	
	public static boolean isNull(Object o) {
		return o == null;
	}	
	
	public static <T> T defaultIfEmpty(T obj, T defaultObj) {
		if (isNull(obj)) return defaultObj;
		return obj;
	}
}
